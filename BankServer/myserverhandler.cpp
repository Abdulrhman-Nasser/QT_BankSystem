
#include "myserverhandler.h"
MyServerHandler::MyServerHandler(qint32 ID, QObject *parent)
    :QThread{parent},id(ID),crypt(0x37769a6a5d4550d2),hashing(QCryptographicHash::Keccak_512)
{
    LoginData.setPath(QDir::currentPath()+"/debug/LoginDB.json");
    HistoryDB.setPath(QDir::currentPath()+"/debug/HistoryDB.json");
    Log.setPath(QDir::currentPath()+"/debug/Log.json");

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
        hashing.reset();
        hashing.addData(byteArray);
        QString header = QString("JsonSize:%1/%2/").arg(byteArray.size()).arg(QString(hashing.result()));
        byteArray.prepend(header.toUtf8());
        QByteArray data = crypt.encryptToByteArray(byteArray);
        qDebug()<<data;
        qDebug()<<crypt.decryptToString(data);
        socket->write(data);
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
        QString name = jsonobjLogin["Name"].toString();
        QString balance = jsonobjLogin["Balance"].toString();
        qint32 accountNumber = QRandomGenerator::global()->bounded(1000, 10000);
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
        jsonObject["Available"] = true;
        jsonObject["Name"] = name;
        jsonObject["Balance"] = balance.toInt();
        jsonarr.append(jsonObject);
        LoginData.writeFile(jsonarr);
        name = username;
        AccNum = accountNumber;
        this->pass=pass.toInt();
        sendMessage("Account Created :D","Success");

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
                QJsonArray transactions=jsonobj["transactions"].toArray();
                if(str.isEmpty() || str.toInt()>transactions.size())
                {
                    QJsonDocument jsondoc(jsonobj["transactions"].toArray());
                    QString data = jsondoc.toJson(QJsonDocument::Compact);
                    sendMessage(data,"History");
                    return;
                }
                else
                {

                    QJsonArray temp;
                    int z = transactions.size()-1;
                    for(int y=0 ; y<str.toInt() ; y++)
                    {
                        temp.append(transactions[z]);
                        z--;
                    }
                    QJsonDocument jsondoc(temp);
                    QString data = jsondoc.toJson(QJsonDocument::Compact);
                    sendMessage(data,"History");
                    return;
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
                    return;
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
                    return;
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

    else if(Type == "GetHistoryAdmin")
    {
        QJsonDocument ReceviedDoc = QJsonDocument::fromJson(str.toUtf8());
        QJsonObject obj = ReceviedDoc.object();
        qint32 count = obj["Count"].toInt();
        qint32 accnum = obj["Account Number"].toInt();
        QJsonArray jsonarr= HistoryDB.readFile();
        if(count == 0 && accnum==0)
        {
            qDebug()<<count<<accnum;
            QJsonDocument jsondoc(jsonarr);
            QString data = jsondoc.toJson(QJsonDocument::Compact);
            sendMessage(data,"AdminHistory");
            return;
        }
        else if(count == 0)
        {
            qDebug()<<count<<accnum;
            for (int i =0 ; i< jsonarr.size() ; i++)
            {
                QJsonObject jsonobj = jsonarr[i].toObject();
                if(accnum == jsonobj["Account Number"].toInt())
                {
                    QJsonArray arr;
                    arr.append(jsonobj);
                    QJsonDocument jsondoc(arr);
                    QString data = jsondoc.toJson(QJsonDocument::Compact);
                    sendMessage(data,"AdminHistory");
                    return;
                }
            }
            sendMessage("Invalid Account Number!","Error");
            return;
        }
        else if(accnum ==0)
        {
            qDebug()<<count<<accnum;
            for (int i =0 ; i< jsonarr.size() ; i++)
            {
                QJsonObject jsonobj = jsonarr[i].toObject();
                QJsonArray transactions=jsonobj["transactions"].toArray();
                QJsonArray temp;
                for(int y = 0 ; y<count && y<transactions.size() ; y++)
                {
                    temp.append(transactions[y]);
                }
                jsonobj["transactions"]=temp;
                jsonarr[i]=jsonobj;
            }
            QJsonDocument jsondoc(jsonarr);
            QString data = jsondoc.toJson(QJsonDocument::Compact);
            sendMessage(data,"AdminHistory");

        }
        else
        {
            qDebug()<<count<<accnum;
            for (int i =0 ; i< jsonarr.size() ; i++)
            {
                QJsonObject jsonobj = jsonarr[i].toObject();
                if(accnum == jsonobj["Account Number"].toInt())
                {
                    QJsonArray transactions=jsonobj["transactions"].toArray();
                    QJsonArray temp;
                    for(int y = 0 ; y<count && y<transactions.size() ; y++)
                    {
                        temp.append(transactions[y]);
                    }
                    jsonobj["transactions"]=temp;
                    QJsonArray arr;
                    arr.append(jsonobj);
                    QJsonDocument jsondoc(arr);
                    QString data = jsondoc.toJson(QJsonDocument::Compact);
                    sendMessage(data,"AdminHistory");
                    return;
                }
            }
            sendMessage("Invalid Account Number!","Error");
        }
    }

    else if (Type == "GetAdminDB")
    {
        if(str.isEmpty())
        {
            QJsonDocument jsondoc(LoginData.readFile());
            QString data = jsondoc.toJson(QJsonDocument::Compact);
            sendMessage(data,"AdminDB");
        }
        else
        {
            qint32 accnum = str.toInt();
            QJsonArray jsonarr = LoginData.readFile();
            for(int i = 0 ;i<jsonarr.size();i++)
            {
                QJsonObject jsonobj = jsonarr[i].toObject();
                if(accnum == jsonobj["Account Number"].toInt())
                {
                    QJsonArray newjsonarr;
                    newjsonarr.append(jsonobj);
                    QJsonDocument jsondocsend(newjsonarr);
                    QString data = jsondocsend.toJson(QJsonDocument::Compact);
                    sendMessage(data,"AdminDB");
                    return;
                }
            }
            sendMessage("Invalid Account Number!","Error");
        }
    }
    else if( Type == "DeleteAccount")
    {
        qint32 accnum = str.toInt();
        QJsonArray jsonarr = LoginData.readFile();
        QJsonArray newarr;
        for(int i=0 ; i<jsonarr.size(); i++)
        {
            QJsonObject jsonobj = jsonarr[i].toObject();
            if(jsonobj["Account Number"].toInt() != accnum)
            {
                newarr.append(jsonobj);
            }
        }
        if(newarr.size() == jsonarr.size())
        {
            sendMessage("Invalid Account Number!","Error");
            return;
        }
        LoginData.writeFile(newarr);
    }
    else if(Type == "Update")
    {
        QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
        QJsonObject jsonobjUpdate = doc.object();
        QString username = jsonobjUpdate["UserName"].toString();
        QString pass = jsonobjUpdate["Password"].toString();
        QString name = jsonobjUpdate["Name"].toString();
        QString balance = jsonobjUpdate["Balance"].toString();
        QString Accnum = jsonobjUpdate["Account Number"].toString();
        qint32 accnum = Accnum.toInt();
        QJsonArray jsonarr = LoginData.readFile();
        for(int i=0 ; i<jsonarr.size(); i++)
        {
            QJsonObject jsonobj = jsonarr[i].toObject();
            if(jsonobj["Account Number"].toInt() == accnum)
            {
                if(!username.isEmpty())
                    jsonobj["UserName"] = username;
                if(!pass.isEmpty())
                    jsonobj["Password"] = pass.toInt();
                if(!name.isEmpty())
                    jsonobj["Name"] = name;
                if(!balance.isEmpty())
                    jsonobj["Balance"] = balance.toInt();
                jsonarr[i]=jsonobj;
                LoginData.writeFile(jsonarr);
                return;
            }
        }
        sendMessage("Invalid Account Number!","Error");
    }
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
    QString str = crypt.decryptToString(byteArray);
    QStringList list = str.split("/");
    QString hash = list[1];
    QString data = list[2];
    QJsonDocument jsonDoc=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObject = jsonDoc.object();
    QString Type = jsonObject["file_type"].toString();
    qint32 dataSize=jsonObject["file_size"].toInt();
    QString realData = jsonObject["file_data"].toString();
    hashing.reset();
    hashing.addData(data.toUtf8());
    if(dataSize == realData.size() && hash == hashing.result())
    {
        qDebug()<<"Received data from client "<<id<<" =>" <<realData;
        QJsonArray arr=Log.readFile();
        QJsonObject obj;
        obj["TimeStamp: "] = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
        obj["Operation: "]= Type;
        obj["Data: "] = realData;
        obj["Account Number: "]=AccNum;
        arr.append(obj);
        Log.writeFile(arr);
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
