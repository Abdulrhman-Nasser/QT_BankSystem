
#include "myclient.h"

MyClient::MyClient(QObject *parent)
    : QObject{parent},crypt(0x37769a6a5d4550d2),hash(QCryptographicHash::Keccak_512)
{
    // connect(&socket,&QTcpSocket::connected,this,&MyClient::onConnection);
    // connect(&socket,&QTcpSocket::disconnected,this,&MyClient::onDisconnection);
    // connect(&socket,&QTcpSocket::errorOccurred,this,&MyClient::onError);
    // connect(&socket,&QTcpSocket::stateChanged,this,&MyClient::onStateChanged);

    connect(&socket,&QTcpSocket::readyRead,this,&MyClient::onReadyRead);


}

void MyClient::Disconnect()
{
    if(socket.isOpen())
    {
        socket.close();
    }
}

void MyClient::ConnectToDevice(QString ip, qint32 port)
{
    if(socket.isOpen())
    {
        if((this->ip == ip) && (this->port))
        {
            return;
        }
        else
        {
            this->ip= ip;
            this->port=port;
            socket.close();
            socket.connectToHost(this->ip,this->port);
        }
    }
    else
    {
        this->ip= ip;
        this->port=port;
        socket.connectToHost(this->ip,this->port);
    }
}

void MyClient::WriteData(QString str,QString Type)
{
    if(socket.isOpen())
    {
        QJsonObject jsonObject;
        jsonObject["file_type"]=Type;
        jsonObject["file_size"]=str.toUtf8().size();
        jsonObject["file_data"]=str;
        QByteArray byteArray= QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
        hash.reset();
        hash.addData(byteArray);
        QString header = QString("JsonSize:%1//?%2//?").arg(byteArray.size()).arg(QString(hash.result()));
        byteArray.prepend(header.toUtf8());
        QByteArray data = crypt.encryptToByteArray(byteArray);
        qDebug()<<data;
        qDebug()<<crypt.decryptToString(data);
        socket.write(data);
    }
}

void MyClient::onReadyRead()
{
    QByteArray byteArray=socket.readAll();
    QString str = crypt.decryptToString(byteArray);
    QStringList list = str.split("//?");
    QString hashed = list[1];
    QString data = list[2];
    QJsonDocument jsonDoc=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObject = jsonDoc.object();
    QString Type = jsonObject["file_type"].toString();
    qint32 dataSize=jsonObject["file_size"].toInt();
    QString realData = jsonObject["file_data"].toString();
    hash.reset();
    hash.addData(data.toUtf8());
    if(dataSize == realData.size() && hashed == hash.result())
    {
        qDebug()<<"Received data from server =>" <<byteArray;
        if(Type == "SuccessLogin")
        {
            emit LoginSuccess(realData);
        }
        else if (Type == "Error")
        {
            emit LoginError(realData);
        }
        else if(Type == "AccNum")
        {
            emit ReceiveAccNum(realData);
        }
        else if(Type == "AccNumAdmin")
        {
            emit ReceiveAccNums(realData);
        }
        else if (Type == "Balance")
        {
            emit Balance(realData);
        }
        else if (Type == "BalanceAdmin")
        {
            emit BalanceAdmin(realData);
        }
        else if(Type == "History")
        {
            emit History(realData);
        }
        else if(Type == "Success")
        {
            emit success(realData);
        }
        else if(Type == "AdminHistory")
        {
            emit AdminHistory(realData);
        }
        else if(Type == "AdminDB")
        {
            emit AdminDB(realData);
        }
        // else if (Type == "Data")
        // {
        //     emit ProductsUpdate(realData);
        // }
        // else if(Type == "Address")
        // {
        //     emit addressUpdate(realData);
        // }
        // else if(Type == "SuccessCheckout")
        // {
        //     emit CheckoutSuccess(realData);
        // }
    }
    else
    {
        qDebug()<<"Something wrong with the data!";
    }
}
