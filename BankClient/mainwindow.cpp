#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&client,&MyClient::LoginError,this,&MainWindow::Error);
    connect(&client,&MyClient::LoginSuccess,this,&MainWindow::SuccessLogin);
    client.ConnectToDevice("192.168.1.2",1234);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SuccessLogin(QString data)
{
    qDebug()<<"Success";
}

void MainWindow::Error(QString data)
{
    QMessageBox::critical(nullptr, "Error", data);
    qDebug()<<"Error";
}

void MainWindow::on_pb_Login_clicked()
{
    QJsonObject jsonobj;
    jsonobj["UserName"]=ui->LE_Loginuser->text();
    jsonobj["UserName"]=ui->LE_Loginuser->text();
    QJsonDocument jsondoc(jsonobj);
    QString data = jsondoc.toJson(QJsonDocument::Compact);
    client.WriteData(data,"Login");
}

