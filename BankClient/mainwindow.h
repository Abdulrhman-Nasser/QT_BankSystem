#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <myclient.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void SuccessLogin(QString);
    void Error(QString);
    void AccountNum(QString);
    void AccountNums(QString);
    void GetBalance(QString);
    void GetHistory(QString);
    void Success(QString);
    void GetHistoryAdmin(QString);
    void GetAdminDB(QString);
private slots:
    void on_pb_Login_clicked();

    void on_pb_signoutadmin_clicked();

    void on_pb_signoutuser_clicked();

    void on_pb_signup_clicked();

    void on_pb_accNumUser_clicked();

    void on_pb_accNumAdmin_clicked();

    void on_pb_UserHome_clicked();

    void on_pb_UserHistory_clicked();

    void on_pb_UserTransaction_clicked();

    void on_pb_UserBalance_clicked();

    void on_pb_UserHistory_2_clicked();

    void on_pb_UserTransaction_2_clicked();

    void on_pb_UserTransfer_clicked();

    void on_pb_AdminHome_clicked();

    void on_pb_AdminHistory_2_clicked();

    void on_pb_AdminManagment_clicked();

    void on_SW_Admin_currentChanged(int arg1);

    void on_SW_User_currentChanged(int arg1);

    void on_pb_AdminDB_clicked();

    void on_pb_AdminHistory_clicked();

    void on_pb_AdminViewDB_clicked();

    void on_pb_delete_clicked();

    void on_pb_Update_clicked();

private:
    void setColor(QString BackColor,QString textColor);
    void ClearText();
    Ui::MainWindow *ui;
    MyClient client;
};
#endif // MAINWINDOW_H
