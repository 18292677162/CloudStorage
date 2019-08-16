#ifndef TRANSFER_H
#define TRANSFER_H

#include <QWidget>
#include "common/common.h"

namespace Ui {
class Transfer;
}

class Transfer : public QWidget
{
    Q_OBJECT

public:
    explicit Transfer(QWidget *parent = nullptr);
    ~Transfer();


    // 显示上传窗口
    void showUpload();
    // 显示下载窗口
    void showDownload();

private:
    Ui::Transfer *ui;
};

#endif // TRANSFER_H
