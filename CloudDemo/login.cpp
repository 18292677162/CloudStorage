#include "login.h"
#include "ui_login.h"
#include <QPainter>
#include <QMessageBox>
#include "common/common.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // 去掉边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    // 设置除ui设置外所有字体
    this->setFont(QFont("宋体", 11, QFont::Normal, false));

    // titlewidget信号处理
    connect(ui->title_wg, &TitleWg::showSetWg, this, [=]()
    {
        // 切换到设置
        ui->stackedWidget->setCurrentWidget(ui->set_page);
        // 清空密码
        ui->passwd_login->clear();
    });

    connect(ui->title_wg, &TitleWg::closeWindow , this, [=]()
    {
        // 判断当前页面 or 首页面
        if(ui->stackedWidget->currentWidget() == ui->set_page)
        {
            // 切换到登录
            ui->stackedWidget->setCurrentWidget(ui->login_page);
        }
        else if(ui->stackedWidget->currentWidget() == ui->reg_page)
        {
            // 切换到登录
            ui->stackedWidget->setCurrentWidget(ui->login_page);
            // 清空数据
            ui->name_reg->clear();
            ui->id_reg ->clear();
            ui->passwd__reg->clear();
            ui->re_passwd_reg->clear();
            ui->phone_reg->clear();
            ui->email_reg->clear();
        }
        else
        {
            this->close();
        }
    });

    // 注册
    connect(ui->sign_up, &QToolButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->reg_page);
        // 设置焦点（默认选取）
        ui->name_reg->setFocus();
    });
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

// 用户注册
void Login::on_signup_button_2_clicked()
{

}

// 服务器设置
void Login::on_toolButton_4_clicked()
{
    // 获取控件数据
    QString ip = ui->ip_set->text();
    QString port = ui->port_set->text();
    // 数据校验 -- 正则表达式
    QRegExp exp(IP_REG);
    if(!exp.exactMatch(ip))
    {
        QMessageBox::warning(this, "警告", "IP格式输入不正确");
        ui->ip_set->clear();
        // 设置焦点
        ui->ip_set->setFocus();
        return;
    }
    // 端口校验 -- regexp
    QRegExp exp1(PORT_REG);
    if(!exp1.exactMatch(port))
    {
        QMessageBox::warning(this, "警告", "端口格式输入不正确");
        ui->port_set->clear();
        // 焦点
        ui->port_set->setFocus();
        return;
    }
}
