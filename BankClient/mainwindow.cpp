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
    connect(&client,&MyClient::AdminHistory,this,&MainWindow::GetHistoryAdmin);
    connect(&client,&MyClient::success,this,&MainWindow::Success);
    connect(&client,&MyClient::AdminDB,this,&MainWindow::GetAdminDB);
    client.ConnectToDevice("192.168.137.243",1234);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SuccessLogin(QString data)
{
    if(data=="Admin")
    {
        ui->SW_Admin->setCurrentIndex(0);
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
    QString UserName = ui->LE_NewUserName->text();
    QString pass = ui->LE_NewPassword->text();
    QString name = ui->LE_NewName->text();
    QString balance = ui->LE_NewBalance->text();
    bool CheckBalance,CheckPass;
    balance.toInt(&CheckBalance);
    pass.toInt(&CheckPass);
    //qDebug()<<CheckBalance<<CheckPass;
    if(UserName.isEmpty() || pass.isEmpty() || name.isEmpty() || balance.isEmpty() || !CheckBalance || !CheckPass)
    {
        QMessageBox::warning(nullptr,"Warning","Enter valid data");
        return;
    }
    jsonobj["UserName"]=UserName;
    jsonobj["Password"]=pass;
    jsonobj["Name"]=name;
    jsonobj["Balance"]=balance;
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

    ui->L_background_2->setStyleSheet("background-color: "+BackColor+";");
    ui->pb_AdminHome->setStyleSheet(
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
    ui->pb_AdminHistory_2->setStyleSheet(
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
    ui->pb_AdminManagment->setStyleSheet(
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
    ui->pb_AdminDB->setStyleSheet(
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
    ui->pb_signoutadmin->setStyleSheet(
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
    ui->L_UserBalance->clear();
    ui->LE_accNumBalance->clear();
    ui->L_AdminBalance->clear();
    ui->LE_AdminHistoryCount->clear();
    ui->LW_AdminHistory->clear();
    ui->LW_AdminHistory->clear();
    //ui->LE_AdminAccNumHis->clear();
    ui->LE_NewBalance->clear();
    ui->LE_NewName->clear();
    ui->LE_NewPassword->clear();
    ui->LE_NewUserName->clear();
    ui->LE_UpdateBalance->clear();
    ui->LE_UpdateName->clear();
    ui->LE_UpdatePassword->clear();
    ui->LE_UpdateUserName->clear();
    ui->LE_UpdateAccNum->clear();
    ui->LE_DeleteAccNum->clear();
    ui->LE_AdminHistoryAccNum->clear();
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

void MainWindow::GetHistoryAdmin(QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonArray history = doc.array();
    qDebug()<<history.size();
    for(int i =0 ; i<history.size();i++)
    {
        QJsonObject jsonobj = history[i].toObject();
        ui->LW_AdminHistory->addItem("*****************");
        ui->LW_AdminHistory->addItem("Username: "+jsonobj["UserName"].toString());
        ui->LW_AdminHistory->addItem("Account number: "+QString::number(jsonobj["Account Number"].toInt()));
        ui->LW_AdminHistory->addItem("-----------------");
        QJsonArray transactionarr = jsonobj["transactions"].toArray();
        for(int y=0; y<transactionarr.size() ; y++)
        {
            QJsonObject transactionobj = transactionarr[y].toObject();
            ui->LW_AdminHistory->addItem("Date of Transaction : "+transactionobj["date"].toString());
            ui->LW_AdminHistory->addItem("Amount : "+QString::number(transactionobj["Amount"].toInt()));
            QString accnum = QString::number(transactionobj["To/From"].toInt());
            if(accnum != "0")
                ui->LW_AdminHistory->addItem("To/From account number : "+accnum);
            ui->LW_AdminHistory->addItem("-----------------");

        }
        ui->LW_AdminHistory->addItem("*****************");
    }
}

void MainWindow::GetAdminDB(QString data)
{
    QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonArray dataBase = jsondoc.array();
    for(int i=0 ; i<dataBase.size() ; i++)
    {
        QJsonObject jsonobj = dataBase[i].toObject();
        QString auth = jsonobj["Auth"].toString();
        if(auth=="User")
        {
            ui->LW_AdminDB->addItem("Name: "+jsonobj["Name"].toString());
            ui->LW_AdminDB->addItem("Balance: "+QString::number(jsonobj["Balance"].toInt()));
            ui->LW_AdminDB->addItem("Account number: "+QString::number(jsonobj["Account Number"].toInt()));
        }
        ui->LW_AdminDB->addItem("Username: "+jsonobj["UserName"].toString());
        ui->LW_AdminDB->addItem("Password: "+QString::number(jsonobj["Password"].toInt()));
        ui->LW_AdminDB->addItem("Authority: "+auth);
        ui->LW_AdminDB->addItem("------------------");
    }
}

void MainWindow::on_pb_UserHistory_2_clicked()
{
    QString History = ui->LE_UserHistoryCount->text();
    bool Check;
    History.toInt(&Check);
    if (Check || History.isEmpty())
        client.WriteData(History,"GetHistory");
    else
        QMessageBox::warning(nullptr,"Warning","Enter valid count number");
}


void MainWindow::on_pb_UserTransaction_2_clicked()
{
    QString Transaction = ui->LE_UserTransaction->text();
    bool Check;
    Transaction.toInt(&Check);
    if (Check)
        client.WriteData(Transaction,"DoTransaction");
    else
        QMessageBox::warning(nullptr,"Warning","Enter valid transaction number");

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


void MainWindow::on_pb_AdminHome_clicked()
{
    ui->SW_Admin->setCurrentIndex(0);
}


void MainWindow::on_pb_AdminHistory_2_clicked()
{
    ui->SW_Admin->setCurrentIndex(1);
}


void MainWindow::on_pb_AdminManagment_clicked()
{
    ui->SW_Admin->setCurrentIndex(3);
}


void MainWindow::on_SW_Admin_currentChanged(int arg1)
{
    ClearText();
}


void MainWindow::on_SW_User_currentChanged(int arg1)
{
    ClearText();
}


void MainWindow::on_pb_AdminDB_clicked()
{
    ui->SW_Admin->setCurrentIndex(2);
}


void MainWindow::on_pb_AdminHistory_clicked()
{
    QString count = ui->LE_AdminHistoryCount->text();
    QString AccNum = ui->LE_AdminHistoryAccNum->text();
    bool CheckCount;
    bool CheckAccNum;
    qint32 countnum=count.toInt(&CheckCount);
    qint32 accnum=AccNum.toInt(&CheckAccNum);
    if( (CheckCount || count.isEmpty()) && (CheckAccNum || AccNum.isEmpty()) )
    {
        QJsonObject jsonobj;
        jsonobj["Count"]=countnum;
        jsonobj["Account Number"]=accnum;
        QJsonDocument jsondoc(jsonobj);
        QString data = jsondoc.toJson(QJsonDocument::Compact);
        client.WriteData(data,"GetHistoryAdmin");
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning","Enter valid Count and Account Number");
    }
}


void MainWindow::on_pb_AdminViewDB_clicked()
{
    QString accnum = ui->LE_AdminAccNumDB->text();
    bool Check;
    qint32 AccNum = accnum.toInt(&Check);
    if(Check || accnum.isEmpty())
    {
        client.WriteData(accnum,"GetAdminDB");
    }
}


void MainWindow::on_pb_delete_clicked()
{
    QString accnum = ui->LE_DeleteAccNum->text();
    bool Check;
    accnum.toInt(&Check);
    if(!Check)
    {
        QMessageBox::warning(nullptr,"Warning","Enter a valid Account Number");
        return;
    }
    client.WriteData(accnum,"DeleteAccount");
}


void MainWindow::on_pb_Update_clicked()
{
    QJsonObject jsonobj;
    QString UserName = ui->LE_UpdateUserName->text();
    QString pass = ui->LE_UpdatePassword->text();
    QString name = ui->LE_UpdateName->text();
    QString balance = ui->LE_UpdateBalance->text();
    QString accnum = ui->LE_UpdateAccNum->text();
    bool CheckBalance,CheckPass,CheckAccNum;
    balance.toInt(&CheckBalance);
    pass.toInt(&CheckPass);
    accnum.toInt(&CheckAccNum);
    if(accnum.isEmpty() || !CheckAccNum)
    {
        QMessageBox::warning(nullptr,"Warning","Enter a valid Account Number");
        return;
    }
    else if((!CheckBalance && !balance.isEmpty()) || (!CheckPass && !pass.isEmpty()))
    {
        QMessageBox::warning(nullptr,"Warning","Enter valid password or balance");
        return;
    }

    jsonobj["UserName"]=UserName;
    jsonobj["Password"]=pass;
    jsonobj["Name"]=name;
    jsonobj["Balance"]=balance;
    jsonobj["Account Number"]=accnum;
    QJsonDocument jsondoc(jsonobj);
    QString data = jsondoc.toJson(QJsonDocument::Compact);
    client.WriteData(data,"Update");
}


void MainWindow::on_pb_AdminBalance_clicked()
{

}

