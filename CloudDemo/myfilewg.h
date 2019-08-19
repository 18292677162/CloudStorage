#ifndef MYFILEWG_H
#define MYFILEWG_H

#include <QWidget>
#include <QTimer>
#include "common/common.h"
#include "common/uploadtask.h"
#include "mymenu.h"

namespace Ui {
class MyFileWg;
}

class MyFileWg : public QWidget
{
    Q_OBJECT

public:
    explicit MyFileWg(QWidget *parent = nullptr);
    ~MyFileWg();

    // 初始化 listWidget 文件列表
    void initListWidget();
    // 添加右键菜单
    void addActionMenu();

    // ----------------------上传文件-------------------------
    // 添加上传文件到任务列表
    void addUploadFiles();
    // 设置MD5 Json 包
    QByteArray setMd5Json(QString user, QString token, QString md5, QString fileName);
    // 上传文件处理，取出队首，上传完毕取下一个
    void uploadFilesAction();
    // 上传文件，非秒传
    void uploadFile(UploadFileInfo *info);

    // ----------------------文件 上传 item------------------------
    // 清空文件列表
    void clearFileList();
    // 清空所有 item 项目
    void clearItems();
    // 添加上传文件项目 item
    void addUploadItem(QString iconPath = ":/image/upload.png", QString = "上传文件");
    // 文件 item 展示
    void refreshFileItems();


    //--------------------------显示用户的文件列表------------------------
    // desc ---> descend   降序
    // asc   ---> ascend       升序
    // Normal：普通用户列表，PvAsc：按下载量升序， PvDesc：按下载量降序
    enum Display{Normal, PvAsc, PvDesc};
    // 得到服务器json文件
    QStringList getCountStatus(QByteArray json);
    // 显示用户的文件列表
    void refreshFiles(Display cmd=Normal);
    // 设置json包
    QByteArray setGetCountJson(QString user, QString token);
    // 设置json包
    QByteArray setFilesListJson(QString user, QString token, int start, int count);
    // 获取用户文件列表
    void getUserFilesList(Display cmd=Normal);
    // 解析文件列表json信息，存放在文件列表中
    void getFileJsonInfo(QByteArray data);


    // 清除上传下载任务
    void clearAllTask();
    // 定时检查处理任务队列中的任务
    void checkTaskList();


    // -------------------------下载文件处理----------------------------
    // 添加需要下载的文件到下载任务列表
    void addDownloadFiles();
    // 下载文件处理，取出下载任务列表的队首任务，下载完后，再取下一个任务
    void downloadFilesAction();

    // 下载文件标志 pv 处理
    void dealFilePv(QString md5, QString filename);



    // ------------------------分享/删除文件------------------------
    // 处理选中的文件
    void dealSelectdFile(QString cmd="分享");
    // 设置删除 json 包
    QByteArray setDealFileJson(QString user, QString token, QString md5, QString filename);

    // 分享文件
    void shareFile(FileInfo *info);

    // 删除文件
    void delFile(FileInfo *info);
    // -------------------------获取文件属性------------------------
    // 获取属性信息
    void getFileProperty(FileInfo *info);


private:
    // 右键菜单信号槽函数
    void rightMenu(const QPoint &pos);

signals:
    void gotoTransfer(TransferStatus status);
    void loginAgainSignal();

private:
    Ui::MyFileWg *ui;

    Common m_cm;
    QNetworkAccessManager* m_manager;

    MyMenu *m_menu;                   // 文件菜单
    QAction *m_downloadAction;      // 下载
    QAction *m_shareAction;              // 分享
    QAction *m_delAction;                   // 删除
    QAction *m_propertyAction;          // 属性

    MyMenu *m_menuEmpty;                // 空白菜单
    QAction *m_pvAscendingAction;     // 按下载量升序
    QAction *m_pvDescendingAction;   // 按下载量降序
    QAction *m_refreshAction;               // 刷新
    QAction *m_uploadAction;               // 上传

    // 文件列表
    QList<FileInfo *> m_fileList;

    long m_userFilesCount;      // 文件数目
    int m_start;                          // 文件位置起点
    int m_count;                        // 单次请求文件个数

    // 定时器
    QTimer m_uploadFileTimer;   // 检查上传队列是否有任务
    QTimer m_downloadTimer;    // 检查下载队列是否有任务

};

#endif // MYFILEWG_H
