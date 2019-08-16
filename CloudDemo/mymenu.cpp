#include "mymenu.h"

MyMenu::MyMenu(QWidget *parent) : QMenu(parent)
{
    setStyleSheet(
            "background-color:rgba(236, 233, 254, 180);"
            "color:rgb(0, 0, 0);"
            "font: 12pt \"微软雅黑\";"
    );
}
