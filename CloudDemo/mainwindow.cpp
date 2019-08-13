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
