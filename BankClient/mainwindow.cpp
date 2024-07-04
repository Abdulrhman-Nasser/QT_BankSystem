#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    connect(&client,&MyClient::LoginError,this,&MainWindow::Error);
    connect(&client,&MyClient::LoginSuccess,this,&MainWindow::SuccessLogin);
    connect(&client,&MyClient::ReceiveAccNum,this,&MainWindow::AccountNum);
    connect(&client,&MyClient::ReceiveAccNums,this,&MainWindow::AccountNums);
    client.ConnectToDevice("192.168.153.158",1234);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SuccessLogin(QString data)
{
    if(data=="Admin")
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    else if(data == "User")
    {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void MainWindow::Error(QString data)
{
    QMessageBox::critical(nullptr, "Error", data);
}

void MainWindow::AccountNum(QString num)
{
    ui->L_UserAcc->setText(num);
}

void MainWindow::AccountNums(QString nums)
{
    ui->L_AdminAcc->setText(nums);
}

void MainWindow::on_pb_Login_clicked()
{
    QJsonObject jsonobj;
    QString name = ui->LE_Loginuser->text();
    QString pass = ui->LE_Loginpass->text();
    if(name.isEmpty() || pass.isEmpty())
    {
        QMessageBox::warning(nullptr,"Warning","Enter valid username and password");
        return;
    }
    jsonobj["UserName"]=name;
    jsonobj["Password"]=pass;
    QJsonDocument jsondoc(jsonobj);
    QString data = jsondoc.toJson(QJsonDocument::Compact);
    client.WriteData(data,"Login");
}


void MainWindow::on_pb_signoutadmin_clicked()
{
    client.WriteData("","SignOut");
    ui->stackedWidget->setCurrentIndex(0);
    ui->LE_Loginpass->clear();
    ui->LE_Loginuser->clear();
    ui->L_AdminAcc->clear();
    ui->LE_accNum->clear();
}


void MainWindow::on_pb_signoutuser_clicked()
{
    client.WriteData("","SignOut");
    ui->stackedWidget->setCurrentIndex(0);
    ui->LE_Loginpass->clear();
    ui->LE_Loginuser->clear();
    ui->L_UserAcc->clear();
}


void MainWindow::on_pb_signup_clicked()
{
    QJsonObject jsonobj;
    QString name = ui->LE_Loginuser->text();
    QString pass = ui->LE_Loginpass->text();
    if(name.isEmpty() || pass.isEmpty())
    {
        QMessageBox::warning(nullptr,"Warning","Enter valid username and password");
        return;
    }
    jsonobj["UserName"]=name;
    jsonobj["Password"]=pass;
    QJsonDocument jsondoc(jsonobj);
    QString data = jsondoc.toJson(QJsonDocument::Compact);
    client.WriteData(data,"Signup");
}


void MainWindow::on_pb_accNumUser_clicked()
{
    client.WriteData("","GetAccNum");
}


void MainWindow::on_pb_accNumAdmin_clicked()
{
    QString accName = ui->LE_accNum->text();
    if(accName.isEmpty())
    {
        QMessageBox::critical(nullptr,"Error","Enter account name");
        return;
    }
    client.WriteData(accName,"GetAccNum");
}

