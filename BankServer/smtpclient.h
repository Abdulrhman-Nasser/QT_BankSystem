#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QObject>
#include <QSslSocket>

class SmtpClient : public QObject {
    Q_OBJECT

public:
    explicit SmtpClient(const QString &host, quint16 port, const QString &username, const QString &password, QObject *parent = nullptr);
    ~SmtpClient();
    void sendEmail(const QString &from, const QString &to, const QString &subject, const QString &body);

private slots:
    void onConnected();
    void onReadyRead();
    void onSslErrors(const QList<QSslError> &errors);
    void onError(QAbstractSocket::SocketError socketError);
signals:
    void deletePointer();
private:
    void sendCommand(const QString &command);

    QSslSocket *socket;
    QString fromEmail;
    QString toEmail;
    QString subject;
    QString body;
    QString username;
    QString password;
    enum State {
        Init, Handshake, Auth, User, Pass, Mail, Rcpt, Data, Body, Quit, Close
    } state;
};

#endif // SMTPCLIENT_H
