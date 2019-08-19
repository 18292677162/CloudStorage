#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <common/common.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 显示主窗口
    void showMainWin();

    // 处理 ButtonGroup 所有信号
    void managerSignals();

    // 重新登录
    void loginAgain();

signals:
    // 切换用户按钮信号
    void changeUser();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;

    Common m_common;
};

#endif // MAINWINDOW_H
