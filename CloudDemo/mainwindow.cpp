#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 去掉边框
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    // 默认显示我的文件窗口
    ui->stackedWidget->setCurrentWidget(ui->myfiles_page);

    // 设置除ui设置外所有字体
    this->setFont(QFont("幼圆", 12, QFont::Normal, false));

    // 处理所有信号
    managerSignals();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMainWin()
{
    m_common.moveToCenter(this); //居中显示
    ui->stackedWidget->setCurrentWidget(ui->myfiles_page);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap bk(":/image/title_bk3.jpg");
    painter.drawPixmap(0, 0, width(), height(), bk);
}

// 处理所有信号
void MainWindow::managerSignals()
{
    // 关闭
    connect(ui->btn_group, &ButtonGroup::closeWindow, this, &MainWindow::close);
    // 最大化
    connect(ui->btn_group, &ButtonGroup::maxWindow, [=]()
    {
        // 全屏为1，小屏为0
        static int flag = 0;
        if(flag)
        {
            this->showNormal();
            flag = 0;
        }
        else
        {
            this->showMaximized();
            flag = 1;
        }
    });
    // 最小化
    connect(ui->btn_group, &ButtonGroup::minWindow, this, &MainWindow::showMinimized);

    // 栈窗口切换
    // 我的文件
    connect(ui->btn_group, &ButtonGroup::sigMyFile, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->myfiles_page);
        // 刷新文件列表

    });

    // 分享列表
    connect(ui->btn_group, &ButtonGroup::sigShareList, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->sharefile_page);
        // 刷新分享列表

    });

    // 下载榜
    connect(ui->btn_group, &ButtonGroup::sigDownload, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->ranking_page);
        // 刷新下载榜列表

    });

    // 传输列表
    connect(ui->btn_group, &ButtonGroup::sigTransform, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->transfer_page);
        // 刷新传输列表

    });

    // 切换用户
    connect(ui->btn_group, &ButtonGroup::sigSwitchUser, [=]()
    {
        loginAgain();
    });
}

// 重新登录
void MainWindow::loginAgain()
{
    // 切换用户
    cout << "exit";
    emit changeUser();
    // 删除上一个用户传输列表
    // 删除上一个用户文件信息
}
