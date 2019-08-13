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

    void showMainWin();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;

    Common m_common;
};

#endif // MAINWINDOW_H
