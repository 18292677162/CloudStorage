#ifndef LOGIN_H
#define LOGIN_H
#include "common/common.h"
#include <mainwindow.h>

#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

protected:
    // 绘图事件函数
    void paintEvent(QPaintEvent *event);

private slots:
    // 注册
    void on_signup_button_2_clicked();

    // 保存服务器设置
    void on_toolButton_4_clicked();

    // 保存配置
    // void saveWebInfo(QString ip, QString port, QString path);

    // 注册信息 to Json
    QByteArray setRegJson(QString user, QString nick, QString pwd, QString phone, QString mail);

    // 登录信息 to Json
    QByteArray setLoginJson(QString user, QString passwd);

    // 获取服务器登录信息
    QStringList getLoginStatus(QByteArray json);

    // 初始化登录界面

    void on_signin_button_clicked();

private:
    // 读取配置信息，设置默认登录状态，默认设置信息
    void readCfg();

private:
    Ui::Login *ui;
    // 主窗口类 指针
    MainWindow* m_mainWin;
    Common m_cm;
};

#endif // LOGIN_H
