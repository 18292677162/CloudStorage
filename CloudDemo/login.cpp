#include "login.h"
#include "ui_login.h"
#include <QPainter>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // 去掉边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    // 设置除ui设置外所有字体
    this->setFont(QFont("宋体", 11, QFont::Normal, false));
}

Login::~Login()
{
    delete ui;
}

void Login::paintEvent(QPaintEvent *event)
{
    // 绘制背景
    QPainter p(this);
    QPixmap pixmap(":/image/login_bk.jpg");
    p.drawPixmap(0, 0, this->width(), this->height(), pixmap);
}
