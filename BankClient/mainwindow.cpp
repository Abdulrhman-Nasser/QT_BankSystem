#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    setColor("#d3ffce","black");
    connect(&client,&MyClient::LoginError,this,&MainWindow::Error);
    connect(&client,&MyClient::LoginSuccess,this,&MainWindow::SuccessLogin);
    connect(&client,&MyClient::ReceiveAccNum,this,&MainWindow::AccountNum);
    connect(&client,&MyClient::ReceiveAccNums,this,&MainWindow::AccountNums);
    connect(&client,&MyClient::Balance,this,&MainWindow::GetBalance);
    connect(&client,&MyClient::History,this,&MainWindow::GetHistory);
    connect(&client,&MyClient::success,this,&MainWindow::Success);
    client.ConnectToDevice("192.168.1.12",1234);
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
        ui->SW_User->setCurrentIndex(0);
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
    ClearText();

}


void MainWindow::on_pb_signoutuser_clicked()
{
    client.WriteData("","SignOut");
    ui->stackedWidget->setCurrentIndex(0);
    ClearText();

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

void MainWindow::setColor(QString BackColor,QString textColor)
{
    ui->L_background->setStyleSheet("background-color: "+BackColor+";");
    ui->pb_UserHome->setStyleSheet(
        QString(
            "QPushButton {"
            "    background-color: %1;"  // Use the variable color
            "    border: none;"
            "    outline: none;"
            "    color: %2;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 0.7);"  // Slightly visible background on hover
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 255, 255, 1);"  // Slightly visible background when pressed
            "}"
            ).arg(BackColor).arg(textColor)
        );
    ui->pb_UserHistory->setStyleSheet(
        QString(
            "QPushButton {"
            "    background-color: %1;"  // Use the variable color
            "    border: none;"
            "    outline: none;"
            "    color: %2;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 0.7);"  // Slightly visible background on hover
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 255, 255, 1);"  // Slightly visible background when pressed
            "}"
            ).arg(BackColor).arg(textColor)
        );
    ui->pb_UserTransaction->setStyleSheet(
        QString(
            "QPushButton {"
            "    background-color: %1;"  // Use the variable color
            "    border: none;"
            "    outline: none;"
            "    color: %2;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 0.7);"  // Slightly visible background on hover
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 255, 255, 1);"  // Slightly visible background when pressed
            "}"
            ).arg(BackColor).arg(textColor)
        );
    ui->pb_signoutuser->setStyleSheet(
        QString(
            "QPushButton {"
            "    background-color: %1;"  // Use the variable color
            "    border: none;"
            "    outline: none;"
            "    color: %2;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 0.7);"  // Slightly visible background on hover
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 255, 255, 1);"  // Slightly visible background when pressed
            "}"
            ).arg(BackColor).arg(textColor)
        );
}

void MainWindow::ClearText()
{
    ui->LE_Loginpass->clear();
    ui->LE_Loginuser->clear();
    ui->L_AdminAcc->clear();
    ui->LE_accNum->clear();
    ui->L_UserAcc->clear();
    ui->LE_UserHistoryCount->clear();
    ui->LE_UserTransaction->clear();
    ui->LE_UserTransferAmount->clear();
    ui->LE_UserTransferToAccName->clear();
    ui->L_AdminAcc->clear();
    ui->L_UserBalance->clear();
}


void MainWindow::on_pb_UserHome_clicked()
{
    ui->SW_User->setCurrentIndex(0);
}


void MainWindow::on_pb_UserHistory_clicked()
{
    ui->SW_User->setCurrentIndex(1);
}


void MainWindow::on_pb_UserTransaction_clicked()
{
    ui->SW_User->setCurrentIndex(2);
}


void MainWindow::on_pb_UserBalance_clicked()
{
    client.WriteData("","GetBalance");
}

void MainWindow::GetBalance(QString balance)
{
    ui->L_UserBalance->setText(balance);
}

void MainWindow::GetHistory(QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonArray history = doc.array();
    for(int i =0 ; i<history.size() ; i++)
    {
        QJsonObject jsonobj = history[i].toObject();
        ui->LW_UserHistory->addItem("Date of Transaction : "+jsonobj["date"].toString());
        ui->LW_UserHistory->addItem("Amount : "+QString::number(jsonobj["Amount"].toInt()));
        QString accnum = QString::number(jsonobj["To/From"].toInt());
        if(accnum != "0")
            ui->LW_UserHistory->addItem("To/From account number : "+accnum);
        ui->LW_UserHistory->addItem("-----------------");
    }
}

void MainWindow::Success(QString msg)
{
    QMessageBox::information(nullptr,"Success",msg);
}

void MainWindow::on_pb_UserHistory_2_clicked()
{
    client.WriteData(ui->LE_UserHistoryCount->text(),"GetHistory");
}


void MainWindow::on_pb_UserTransaction_2_clicked()
{
    client.WriteData(ui->LE_UserTransaction->text(),"DoTransaction");
}


void MainWindow::on_pb_UserTransfer_clicked()
{
    bool checkamount,checkaccnum;
    qint32 amount = ui->LE_UserTransferAmount->text().toInt(&checkamount);
    qint32 AccNum = ui->LE_UserTransferToAccName->text().toInt(&checkaccnum);
    if(checkaccnum && checkamount)
    {
        QJsonObject jsonobj;
        if(amount<=0)
        {
            QMessageBox::warning(nullptr,"Warning","Please enter valid amount");
            return;
        }
        jsonobj["Amount"]=amount;
        jsonobj["Account Number"]=AccNum;
        QJsonDocument jsondoc(jsonobj);
        QString data = jsondoc.toJson(QJsonDocument::Compact);
        client.WriteData(data,"DoTransfer");
    }

}

