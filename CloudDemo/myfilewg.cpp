#include "myfilewg.h"
#include "ui_myfilewg.h"

MyFileWg::MyFileWg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFileWg)
{
    ui->setupUi(this);

}

MyFileWg::~MyFileWg()
{
    delete ui;
}
