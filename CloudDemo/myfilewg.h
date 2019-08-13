#ifndef MYFILEWG_H
#define MYFILEWG_H

#include <QWidget>
#include <QTimer>
#include "common/common.h"

namespace Ui {
class MyFileWg;
}

class MyFileWg : public QWidget
{
    Q_OBJECT

public:
    explicit MyFileWg(QWidget *parent = nullptr);
    ~MyFileWg();

private:
    Ui::MyFileWg *ui;
};

#endif // MYFILEWG_H
