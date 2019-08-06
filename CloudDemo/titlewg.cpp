#include "titlewg.h"
#include "ui_titlewg.h"

#include <QMouseEvent>

TitleWg::TitleWg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleWg)
{
    ui->setupUi(this);

    // logo 图片
    ui->logo->setPixmap(QPixmap(":/image/logo.ico").scaled(40, 40));

    ui->wgtitle->setStyleSheet("color:rgb(255, 255, 255)");

    m_parent = parent;

    // 按钮功能实现
    // 设置
    connect(ui->set, &QToolButton::clicked, this, [=]()
    {
        // 发送自定义信号
        emit showSetWg();
    });

    // 最小化
    connect(ui->min, &QToolButton::clicked, this, [=]()
    {
        m_parent->showMinimized();
    });

    // 关闭
    connect(ui->close, &QToolButton::clicked, this, [=]()
    {
        emit closeWindow();
    });
}

TitleWg::~TitleWg()
{
    delete ui;
}

void TitleWg::mouseMoveEvent(QMouseEvent *event)
{
    // 左键生效     //button 点击  buttons 长按
    if(event->buttons() & Qt::LeftButton)
    {
        // 程序窗口和鼠标移动
        m_parent->move(event->globalPos() - m_pt);
    }
}

void TitleWg::mousePressEvent(QMouseEvent *ev)
{
    // 左键按下
    if(ev->button() == Qt::LeftButton)
    {
        // 求差值 = 屏幕鼠标当前位置 - 程序窗口左上角点相对于屏幕位置
        m_pt = ev->globalPos() - m_parent->geometry().topLeft();    //geometry 获取当前窗口坐标
    }
}
