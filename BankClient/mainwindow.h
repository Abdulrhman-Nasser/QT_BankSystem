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

private slots:
    void on_pb_Login_clicked();

    void on_pb_signoutadmin_clicked();

    void on_pb_signoutuser_clicked();

    void on_pb_signup_clicked();

private:
    Ui::MainWindow *ui;
    MyClient client;
};
#endif // MAINWINDOW_H
