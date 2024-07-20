#include "SmtpClient.h"
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include <QTimer>

SmtpClient::SmtpClient(const QString &host, quint16 port, const QString &username, const QString &password, QObject *parent)
    : QObject(parent), socket(new QSslSocket(this)), state(Init), username(username), password(password) {
    connect(socket, &QSslSocket::connected, this, &SmtpClient::onConnected);
    connect(socket, &QSslSocket::readyRead, this, &SmtpClient::onReadyRead);
    connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this, &SmtpClient::onSslErrors);
    connect(socket, &QSslSocket::errorOccurred, this, &SmtpClient::onError);

    qDebug() << "Attempting to connect to SMTP server...";
    socket->connectToHostEncrypted(host, port);

    connect(socket, &QSslSocket::stateChanged, this, [](QSslSocket::SocketState state) {
        qDebug() << "Socket state changed:" << state;
    });

    // Timeout check
    QTimer::singleShot(10000, this, [this]() {
        if (socket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "Connection timed out.";
            socket->abort();
        }
    });
}

SmtpClient::~SmtpClient()
{
    qDebug()<<"Destroying!";
}

void SmtpClient::sendEmail(const QString &from, const QString &to, const QString &subject, const QString &body) {
    fromEmail = from;
    toEmail = to;
    this->subject = subject;
    this->body = body;
    state = Handshake;
}

void SmtpClient::onConnected() {
    qDebug() << "Connected to server";
    sendCommand("HELO localhost");
}

void SmtpClient::onReadyRead() {
    QTextStream in(socket);
    QString response = in.readAll();
    qDebug() << "Server:" << response;

    switch (state) {
    case Handshake:
        if (response.startsWith("220")) {
            sendCommand("HELO localhost");
            state = Auth;
        }
        break;
    case Auth:
        if (response.startsWith("250")) {
            sendCommand("AUTH LOGIN");
            state = User;
        }
        break;
    case User:
        if (response.startsWith("334")) {
            sendCommand(QByteArray(username.toUtf8()).toBase64());
            state = Pass;
        }
        break;
    case Pass:
        if (response.startsWith("334")) {
            sendCommand(QByteArray(password.toUtf8()).toBase64());
            state = Mail;
        }
        break;
    case Mail:
        if (response.startsWith("235")) {
            sendCommand("MAIL FROM:<" + fromEmail + ">");
            state = Rcpt;
        }
        break;
    case Rcpt:
        if (response.startsWith("250")) {
            sendCommand("RCPT TO:<" + toEmail + ">");
            state = Data;
        }
        break;
    case Data:
        if (response.startsWith("250")) {
            sendCommand("DATA");
            state = Body;
        }
        break;
    case Body:
        if (response.startsWith("354")) {
            QTextStream out(socket);
            out << "From: " << fromEmail << "\r\n";
            out << "To: " << toEmail << "\r\n";
            out << "Subject: " << subject << "\r\n";
            out << "\r\n";
            out << body << "\r\n";
            out << ".\r\n";
            state = Quit;
        }
        break;
    case Quit:
        if (response.startsWith("250")) {
            sendCommand("QUIT");
            state = Close;
        }
        break;
    case Close:
        if (response.startsWith("221")) {
            socket->disconnectFromHost();
            emit deletePointer();
        }
        break;
    default:
        qDebug() << "Unexpected state or response";
        break;
    }
}

void SmtpClient::onSslErrors(const QList<QSslError> &errors) {
    qWarning() << "SSL Errors occurred:" << errors;
    // Do not ignore SSL errors in production code
    socket->ignoreSslErrors();
}

void SmtpClient::onError(QAbstractSocket::SocketError socketError) {
    qDebug() << "Socket error occurred:" << socketError << socket->errorString();
}

void SmtpClient::sendCommand(const QString &command) {
    qDebug() << "Client:" << command;
    QTextStream(socket) << command << "\r\n";
}
