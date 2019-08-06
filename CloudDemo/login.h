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
    void on_signup_button_2_clicked();

    void on_toolButton_4_clicked();

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
