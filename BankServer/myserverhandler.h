#ifndef MYSERVERHANDLER_H
#define MYSERVERHANDLER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QDir>
#include <QDate>
#include <QDateTime>
#include "json.h"
class MyServerHandler : public QThread
{
    Q_OBJECT
public:
    explicit MyServerHandler(qint32 ID,QObject *parent = nullptr);
    void sendMessage(QString,QString);
    void Operations(QString,QString);
    void Login(QString username,qint32 pass);
    void UpdateHistory(qint32 amount,QString username,qint32 FromAccNum,QString ToAccNum="None",QString date = QDate::currentDate().toString("dd/MM/yyyy"));
public slots:
    void onReadyRead();
    void onDisconnected();
private:
    qint32 id;
    QTcpSocket* socket;
    Json LoginData;
    Json HistoryDB;
    Json Log;
    QString name;
    qint32 pass;
    qint32 AccNum;
    // QThread interface
protected:
    void run();

};

#endif // MYSERVERHANDLER_H
