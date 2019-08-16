#include "transfer.h"
#include "ui_transfer.h"
#include <QIcon>
#include "common/uploadlayout.h"

Transfer::Transfer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Transfer)
{
    ui->setupUi(this);

    // 设置上传布局实例
    UploadLayout *uploadLayout = UploadLayout::getInstance();
    uploadLayout->setUploadLayout(ui->upload_scroll);

}

Transfer::~Transfer()
{
    delete ui;
}

// 显示上传窗口
void Transfer::showUpload()
{
    ui->tabWidget->setCurrentWidget(ui->upload);
}

// 显示下载窗口
void Transfer::showDownload()
{
    ui->tabWidget->setCurrentWidget(ui->download);
}
