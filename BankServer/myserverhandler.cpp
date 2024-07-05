
#include "myserverhandler.h"
MyServerHandler::MyServerHandler(qint32 ID, QObject *parent)
    :id(ID), QThread{parent}
{
    LoginData.setPath(QDir::currentPath()+"/debug/LoginDB.json");
    HistoryDB.setPath(QDir::currentPath()+"/debug/HistoryDB.json");

}

void MyServerHandler::sendMessage(QString str,QString Type)
{
    if(socket->isOpen())
    {
        QJsonObject jsonObject;
        jsonObject["file_type"]=Type;
        jsonObject["file_size"]=str.toUtf8().size();
        jsonObject["file_data"]=str;
        QByteArray byteArray= QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
        QString header = QString("JsonSize:%1/").arg(byteArray.size());
        byteArray.prepend(header.toUtf8());
        qDebug()<<byteArray;
        socket->write(byteArray);
    }
}

void MyServerHandler::Operations(QString str,QString Type)
{

    if(Type == "Login")
    {
        QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
        QJsonObject jsonobjLogin = doc.object();
        QString username = jsonobjLogin["UserName"].toString();
        QString pass = jsonobjLogin["Password"].toString();
        bool check;
        pass.toInt(&check);
        if(check)
            Login(username,pass.toInt());
        else
            sendMessage("Invalid Password!","Error");
    }
    else if(Type == "SignOut")
    {
        QJsonArray jsonarr=LoginData.readFile();
        for(int i=0 ; i<jsonarr.size();i++)
        {
            QJsonObject jsonobj= jsonarr[i].toObject();
            QString user = jsonobj["UserName"].toString();
            qint32 password = jsonobj["Password"].toInt();
            if(user == name && password == pass)
            {
                jsonobj["Available"] = true;
                jsonarr[i]=jsonobj;
                LoginData.writeFile(jsonarr);
                name.clear();
                pass=0;
                AccNum = 0;
                return;
            }
        }
    }

    //  Signup
    else if(Type ==  "Signup")
    {
        QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
        QJsonObject jsonobjLogin = doc.object();
        QString username = jsonobjLogin["UserName"].toString();
        QString pass = jsonobjLogin["Password"].toString();
        qint32 accountNumber = QRandomGenerator::global()->bounded(1000, 10000);
        bool check;
        pass.toInt(&check);
        if(check)
        {
            QJsonArray jsonarr=LoginData.readFile();
            for(int i=0 ; i<jsonarr.size();i++)
            {
                QJsonObject jsonobj= jsonarr[i].toObject();
                QString user = jsonobj["UserName"].toString();
                qint32 password = jsonobj["Password"].toInt();
                if(user == username && pass.toInt() == password)
                {
                    sendMessage("User already exists!","Error");
                    return;
                }
                if(accountNumber == jsonobj["Account Number"].toInt())
                {
                    accountNumber = QRandomGenerator::global()->bounded(1000, 10000);
                    i=0;
                }
            }
            QJsonObject jsonObject;
            jsonObject["Auth"] = "User";
            jsonObject["Password"] = pass.toInt();
            jsonObject["UserName"] = username;
            jsonObject["Account Number"] = accountNumber;
            jsonObject["Available"] = false;
            jsonarr.append(jsonObject);
            LoginData.writeFile(jsonarr);
            name = username;
            AccNum = accountNumber;
            this->pass=pass.toInt();
            sendMessage("User","SuccessLogin");
        }
        else
            sendMessage("Invalid Password!","Error");
    }

    else if(Type == "GetAccNum")
    {
        QJsonArray jsonarr=LoginData.readFile();
        if (str.isEmpty())
        {
            for(int i=0 ; i<jsonarr.size();i++)
            {
                QJsonObject jsonobj= jsonarr[i].toObject();
                QString user = jsonobj["UserName"].toString();
                qint32 password = jsonobj["Password"].toInt();
                if(user == name && pass == password)
                {
                    sendMessage(QString::number(jsonobj["Account Number"].toInt()),"AccNum");
                    return;
                }
            }
            sendMessage("Account not found!","Error");
        }
        else
        {
            QString accNums;
            for(int i=0 ; i<jsonarr.size();i++)
            {
                QJsonObject jsonobj= jsonarr[i].toObject();
                QString user = jsonobj["UserName"].toString();
                QString Authority = jsonobj["Auth"].toString();
                if(user == str && Authority=="User")
                {
                    accNums.append(QString::number(jsonobj["Account Number"].toInt()));
                    accNums.append("\n");
                }
            }
            if(accNums.isEmpty())
            {
                sendMessage("Account not found!","Error");
                return;
            }
            sendMessage(accNums,"AccNumAdmin");

        }
    }

    else if(Type == "GetBalance")
    {

        QJsonArray jsonarr=LoginData.readFile();
        for(int i=0 ; i<jsonarr.size();i++)
        {
            QJsonObject jsonobj= jsonarr[i].toObject();
            QString user = jsonobj["UserName"].toString();
            qint32 password = jsonobj["Password"].toInt();
            if(user == name && password==pass)
            {
                qint32 balance = jsonobj["Balance"].toInt();
                sendMessage(QString::number(balance),"Balance");
                return;
            }
        }
        sendMessage("No Account!","Error");


    }

    else if (Type == "GetHistory")
    {
        QJsonArray jsonarr= HistoryDB.readFile();
        for(int i =0 ; i<jsonarr.size(); i++)
        {
            QJsonObject jsonobj = jsonarr[i].toObject();
            if(jsonobj["UserName"].toString() == name && jsonobj["Account Number"].toInt() == AccNum)
            {
                if(str.isEmpty())
                {
                    QJsonDocument jsondoc(jsonobj["transactions"].toArray());
                    QString data = jsondoc.toJson(QJsonDocument::Compact);
                    sendMessage(data,"History");
                }
                else
                {
                    QJsonArray transactions=jsonobj["transactions"].toArray();
                    QJsonArray temp;
                    for(int y=0 ; y<str.toInt(); y++)
                    {
                        temp.append(transactions[y]);
                    }
                    QJsonDocument jsondoc(temp);
                    QString data = jsondoc.toJson(QJsonDocument::Compact);
                    sendMessage(data,"History");
                }

            }

        }
    }

    else if(Type == "DoTransaction")
    {

        qint32 amount = str.toInt();
        QJsonArray jsonarr=LoginData.readFile();

        for(int i=0 ; i<jsonarr.size();i++)
        {
            QJsonObject jsonobj= jsonarr[i].toObject();
            QString user = jsonobj["UserName"].toString();
            qint32 password = jsonobj["Password"].toInt();
            if(user == name && pass == password)
            {
                qint32 temp = jsonobj["Balance"].toInt() + amount;
                if( temp > 0 )
                {
                    jsonobj["Balance"]=temp;
                    jsonarr[i]=jsonobj;
                    LoginData.writeFile(jsonarr);
                    sendMessage("Transaction Completed successfully","Success");
                    UpdateHistory(amount,name,AccNum);
                    return;
                }
                else
                {
                    sendMessage("Not enough balance!","Error");
                }

            }
        }
        sendMessage("Account not found!","Error");
    }

    else if(Type == "DoTransfer")
    {
        QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
        QJsonObject transfer = doc.object();
        qint32 amount = transfer["Amount"].toInt();
        qint32 accnum = transfer["Account Number"].toInt();
        if(accnum == AccNum)
        {
            sendMessage("Can't transfer to your account!","Error");
            return;
        }
        QJsonArray jsonarr=LoginData.readFile();
        bool foundAcc = false;
        qint32 id;
        for(int i=0 ; i<jsonarr.size();i++)
        {
            QJsonObject jsonobj= jsonarr[i].toObject();
            QString user = jsonobj["UserName"].toString();
            qint32 password = jsonobj["Password"].toInt();
            qint32 num = jsonobj["Account Number"].toInt();
            if(num == accnum)
            {
                foundAcc=true;
                id = i;
            }
            if(user == name && pass == password)
            {
                qint32 temp = jsonobj["Balance"].toInt() - amount;
                if( temp > 0 )
                {
                    jsonobj["Balance"]=temp;
                    jsonarr[i]=jsonobj;
                }
                else
                {
                    sendMessage("Not enough balance!","Error");
                }

            }
        }
        if(foundAcc)
        {
            QJsonObject obj=jsonarr[id].toObject();
            obj["Balance"]=obj["Balance"].toInt()+amount;
            jsonarr[id]=obj;
            UpdateHistory(amount*-1,name,AccNum,QString::number(obj["Account Number"].toInt()));
            UpdateHistory(amount,obj["UserName"].toString(),obj["Account Number"].toInt(),QString::number(AccNum));
            LoginData.writeFile(jsonarr);
            sendMessage("Transfer Completed successfully","Success");
        }
        else
        {
            sendMessage("Can't find account number!","Error");
        }

    }
    // else if(Type == "GetDataBase")
    // {
    //     QJsonArray jsonarr = ProductsDB.readFile();
    //     QJsonDocument doc;
    //     doc.setArray(jsonarr);
    //     QString data(doc.toJson());
    //     sendMessage(data,"Data");
    // }



    // else if(Type ==  "CheckOut")
    // {
    //     QJsonArray jsonarr=LoginData.readFile();
    //     QString city;
    //     QString gov;
    //     QString dis;
    //     QString building;
    //     QString apt;
    //     QString email;
    //     qint32 number;
    //     for(int i=0 ; i<jsonarr.size();i++)
    //     {
    //         QJsonObject jsonobj= jsonarr[i].toObject();
    //         QString user = jsonobj["UserName"].toString();
    //         qint32 password = jsonobj["Password"].toInt();
    //         QString Authority = jsonobj["Auth"].toString();
    //         if(user == name && this->pass == password)
    //         {
    //             city = jsonobj["City"].toString();
    //             gov = jsonobj["Governorate"].toString();
    //             dis = jsonobj["District"].toString();
    //             building = jsonobj["Building no."].toString();
    //             apt = jsonobj["Apt no."].toString();
    //             email = jsonobj["Email"].toString();
    //             number = jsonobj["Phone Number"].toInt();
    //             QString data=city+"\n"+gov+"\n"+dis+"\n"+building+"\n"+apt+"\n"+email+"\n"+QString::number(number);
    //             qDebug()<<data;
    //             if(data=="\n\n\n\n\n\n0")
    //             {
    //                 sendMessage("No Address Found","Error");
    //                 return;
    //             }
    //         }

    //     }

    //     QJsonArray jsonarrproducts = ProductsDB.readFile();
    //     QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
    //     QJsonArray jsonarrcheckout = doc.array();
    //     // Iterate through each item in the checkout array
    //     for (int i =0 ; i<jsonarrcheckout.size() ; i++) {
    //         QJsonObject checkoutObject= jsonarrcheckout[i].toObject();
    //         qDebug()<<"Before => "<<checkoutObject["id"].toInt();
    //         qint32 checkoutId = checkoutObject["id"].toInt();
    //         qint32 checkoutQuantity = checkoutObject["quantity"].toInt();

    //         // Find the corresponding item in the product array by id
    //         for (int i = 0; i < jsonarrproducts.size(); ++i) {
    //             QJsonObject productObject = jsonarrproducts[i].toObject();
    //             if (productObject["id"] == checkoutId) {
    //                 // Subtract the quantity
    //                 int productQuantity = productObject["quantity"].toInt();
    //                 qDebug()<<"product quantity => "<<productQuantity;
    //                 productObject["quantity"] = productQuantity - checkoutQuantity;
    //                 // Update the product array with the modified object
    //                 jsonarrproducts[i] = productObject;
    //                 break;
    //             }
    //         }
    //     }
    //     ProductsDB.writeFile(jsonarrproducts);
    //     sendMessage("","SuccessCheckout");

    // }

    //  Address save
    // else if(Type ==  "Save_Address")
    // {
    //     QStringList list = str.split('\n');
    //     QString city = list[0];
    //     QString gov=list[1];
    //     QString dis=list[2];
    //     QString building=list[3];
    //     QString apt = list[4];
    //     QString email = list[5];
    //     qint32 number = list[6].toInt();
    //     qDebug()<<city<<gov<<dis<<building<<apt;
    //     QJsonArray jsonarr=LoginData.readFile();
    //     for(int i=0 ; i<jsonarr.size();i++)
    //     {
    //         QJsonObject jsonobj= jsonarr[i].toObject();
    //         QString user = jsonobj["UserName"].toString();
    //         qint32 password = jsonobj["Password"].toInt();
    //         QString Authority = jsonobj["Auth"].toString();
    //         if(user == name && pass == password)
    //         {
    //             jsonobj["City"]=city;
    //             jsonobj["Governorate"]=gov;
    //             jsonobj["District"]=dis;
    //             jsonobj["Building no."]=building;
    //             jsonobj["Apt no."]=apt;
    //             jsonobj["Email"]=email;
    //             jsonobj["Phone Number"]=number;
    //             jsonarr[i]=jsonobj;
    //             break;
    //         }
    //     }
    //     LoginData.writeFile(jsonarr);
    // }
    // else if(Type ==  "Get_Address")
    // {
    //     QJsonArray jsonarr=LoginData.readFile();
    //     for(int i=0 ; i<jsonarr.size();i++)
    //     {
    //         QJsonObject jsonobj= jsonarr[i].toObject();
    //         QString user = jsonobj["UserName"].toString();
    //         qint32 password = jsonobj["Password"].toInt();
    //         QString Authority = jsonobj["Auth"].toString();
    //         QString city = jsonobj["City"].toString();
    //         QString gov = jsonobj["Governorate"].toString();
    //         QString dis = jsonobj["District"].toString();
    //         QString building = jsonobj["Building no."].toString();
    //         QString apt = jsonobj["Apt no."].toString();
    //         QString email = jsonobj["Email"].toString();
    //         qint32 number = jsonobj["Phone Number"].toInt();
    //         if(user == name && this->pass == password)
    //         {
    //             QString data=city+"\n"+gov+"\n"+dis+"\n"+building+"\n"+apt+"\n"+email+"\n"+QString::number(number);
    //             sendMessage(data,"Address");
    //         }

    //     }
    // }
    // else if(Type ==  "Get_Name")
    // {
    //     sendMessage(name,"Name");
    // }
    // else if(Type == "New_Product")
    // {
    //     QJsonArray jsonarrproducts = ProductsDB.readFile();
    //     QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    //     QJsonObject newjsonobj = jsonDoc.object();
    //     for(int i = 0 ; i<jsonarrproducts.size(); i++)
    //     {
    //         QJsonObject jsonobj = jsonarrproducts[i].toObject();
    //         if(jsonobj["id"] == newjsonobj["id"])
    //         {
    //             sendMessage("ID already exists!","Error");
    //             return;
    //         }

    //     }

    //     jsonarrproducts.append(newjsonobj);
    //     ProductsDB.writeFile(jsonarrproducts);
    // }
    // else if(Type == "Delete_Product")
    // {
    //     QJsonArray jsonarrproducts = ProductsDB.readFile();
    //     QJsonArray newJsonArrProducts;
    //     QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    //     QJsonObject jsonobj = jsonDoc.object();
    //     for(int i = 0; i<jsonarrproducts.size() ; i++ )
    //     {
    //         QJsonObject temp = jsonarrproducts[i].toObject();
    //         if(( (temp["id"]!= jsonobj["id"]) && (temp["name"] != jsonobj["name"]) ) )
    //         {
    //             newJsonArrProducts.append(temp);
    //         }
    //     }
    //     ProductsDB.writeFile(newJsonArrProducts);
    // }
    else
    {
        qDebug()<<"Type -> "<<Type<<" is unknown!..";
    }
}

void MyServerHandler::Login(QString username, qint32 pass)
{
    qDebug()<<"Attempting to login with user name -> "<<username<<" and password -> "<<pass;
    QJsonArray jsonarr=LoginData.readFile();
    for(int i=0 ; i<jsonarr.size();i++)
    {
        QJsonObject jsonobj= jsonarr[i].toObject();
        QString user = jsonobj["UserName"].toString();
        qint32 password = jsonobj["Password"].toInt();
        qint32 accNum = jsonobj["Account Number"].toInt();
        QString Authority = jsonobj["Auth"].toString();
        bool available = jsonobj["Available"].toBool();
        if(user == username && pass == password)
        {
            if(available)
            {
                jsonobj["Available"]=false;
                jsonarr[i]=jsonobj;
                LoginData.writeFile(jsonarr);
                qDebug()<<"Login success with authority -> "<<Authority;
                name = username;
                this->pass=pass;
                AccNum = accNum;
                sendMessage(Authority,"SuccessLogin");
                return;
            }
            else
            {
                sendMessage("User in use!","Error");
                return;
            }
        }

    }
    qDebug()<<"Login failed!";
    sendMessage("User Not Found!","Error");
}

void MyServerHandler::UpdateHistory(qint32 amount, QString username, qint32 FromAccNum, QString ToAccNum, QString date)
{
    QJsonArray jsonarr = HistoryDB.readFile();
    for (int i = 0 ; i<jsonarr.size() ; i++)
    {
        QJsonObject jsonobj = jsonarr[i].toObject();
        QString user = jsonobj["UserName"].toString();
        qint32 accnum = jsonobj["Account Number"].toInt();
        if(user == username && accnum == FromAccNum)
        {
            QJsonObject newtransaction;
            newtransaction["date"]=date;
            newtransaction["Amount"]=amount;
            if(ToAccNum!="None")
                newtransaction["To/From"]=ToAccNum.toInt();
            QJsonArray transactions = jsonobj["transactions"].toArray();
            transactions.append(newtransaction);
            jsonobj["transactions"]=transactions;
            jsonarr[i] = jsonobj;
            HistoryDB.writeFile(jsonarr);
            return;
        }
    }
    QJsonObject newtransaction;
    newtransaction["date"]=date;
    newtransaction["Amount"]=amount;
    QJsonArray transactions;
    transactions.append(newtransaction);
    QJsonObject newobj;
    newobj["UserName"]=username;
    newobj["Account Number"]=FromAccNum;
    newobj["transactions"]=transactions;
    jsonarr.append(newobj);
    HistoryDB.writeFile(jsonarr);
}

void MyServerHandler::onReadyRead()
{
    QByteArray byteArray=socket->readAll();
    QString str = QString(byteArray);
    qint32 jsonSize=str.split("/")[0].split(":")[1].toInt();
    QString data = str.right(jsonSize);
    QJsonDocument jsonDoc=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObject = jsonDoc.object();
    QString Type = jsonObject["file_type"].toString();
    qint32 dataSize=jsonObject["file_size"].toInt();
    QString realData = jsonObject["file_data"].toString();
    if(dataSize == realData.size())
    {
        qDebug()<<"Received data from client "<<id<<" =>" <<realData;
        Operations(realData,Type);
    }
    else
    {
        qDebug()<<"Something wrong with the data!";
    }

}

void MyServerHandler::onDisconnected()
{
    if(socket->isOpen())
    {
        socket->close();
        qDebug()<<"Client "<<id<<" has disconnected... ";
        Operations("","SignOut");
    }
}

void MyServerHandler::run()
{
    qDebug()<<"Client => "<<id<<" is running on thread =>"<<QThread::currentThreadId();
    socket=new QTcpSocket;
    socket->setSocketDescriptor(id);
    connect(socket,&QTcpSocket::readyRead,this,&MyServerHandler::onReadyRead,Qt::DirectConnection);
    connect(socket,&QTcpSocket::disconnected,this,&MyServerHandler::onDisconnected,Qt::DirectConnection);
    sendMessage("Hello from my server dear client....","Message");
    exec(); //  Start event loop
}
