
#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include "simplecrypt.h"
class MyClient : public QObject
{
    Q_OBJECT
    QString ip;
    qint32 port;
    QTcpSocket socket;
    SimpleCrypt crypt;
    QCryptographicHash hash;
public:
    explicit MyClient(QObject *parent = nullptr);
    void Disconnect();
    void ConnectToDevice(QString ip,qint32 port);
    void WriteData(QString str,QString Type);
signals:
    void LoginSuccess(QString);
    void LoginError(QString);
    void ReceiveAccNum(QString);
    void ReceiveAccNums(QString);
    void Balance(QString);
    void History(QString);
    void success(QString);
    void AdminHistory(QString);
    void AdminDB(QString);
    void BalanceAdmin(QString);
private slots:
    // void onConnection();
    // void onDisconnection();
    // void onError(QAbstractSocket::SocketError socketError);
    // void onStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();
};

#endif // MYCLIENT_H
