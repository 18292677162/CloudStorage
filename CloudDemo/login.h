#ifndef LOGIN_H
#define LOGIN_H

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
    void saveWebInfo(QString ip, QString port, QString path);

    // 注册信息 to Json
    QByteArray getRegJson(QString user, QString nick, QString pwd, QString phone, QString mail);

    // 初始化登录界面

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
