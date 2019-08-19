#include "myfilewg.h"
#include "ui_myfilewg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QListWidgetItem>
#include "common/logininfoinstance.h"
#include "common/uploadtask.h"
#include "common/downloadtask.h"
#include "filepropertyinfo.h"

MyFileWg::MyFileWg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFileWg)
{
    ui->setupUi(this);

    // 初始化文件listWidget列表
    initListWidget();

    // 添加右键菜单
    addActionMenu();

    // 定时检查任务
    checkTaskList();

    m_manager = Common::getNetManager();
}

MyFileWg::~MyFileWg()
{
    delete ui;
}

void MyFileWg::initListWidget()
{
    ui->listWidget->setViewMode(QListView::IconMode);   //设置显示图标模式
    ui->listWidget->setIconSize(QSize(80, 80));         //设置图标大小
    ui->listWidget->setGridSize(QSize(100, 120));       //设置item大小

    // 设置QLisView大小改变时，图标的调整模式，默认是固定的，可以改成自动调整
    ui->listWidget->setResizeMode(QListView::Adjust);   //自动适应布局

    // 设置列表可以拖动，想不能拖动，使用QListView::Static
    ui->listWidget->setMovement(QListView::Static);
    // 设置图标之间的间距, 当setGridSize()时，此选项无效
    ui->listWidget->setSpacing(30);

    // listWidget 右键菜单
    // 发出 customContextMenuRequested 信号
        ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MyFileWg::rightMenu);
        // 点中列表中的上传文件图标
        connect(ui->listWidget, &QListWidget::itemPressed, this, [=](QListWidgetItem *item)
        {
            QString str = item->text();
            if(str == "上传文件")
            {
                //添加需要上传的文件到上传任务列表
                addUploadFiles();
            }
        });
}

// 显示右键菜单
void MyFileWg::rightMenu(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget->itemAt(pos);

    // 点空白
    if(item == NULL)
    {
        // QPoint QMouseEvent::pos()             返回相对这个widget(重载了QMouseEvent的widget)的位置。
        // QPoint QMouseEvent::globalPos()   窗口坐标，返回鼠标的全局坐标
        // QPoint QCursor::pos() [static]           返回相对显示器的全局坐标
        // QWidget::pos() : QPoint                    获得当前目前控件在父窗口中的位置
        m_menuEmpty->exec( QCursor::pos() );    //在鼠标点击的地方弹出菜单
    }
    // 点图标
    else
    {
        ui->listWidget->setCurrentItem(item);
        if(item->text() == "上传文件")
        {
            return;
        }

        m_menu->exec(QCursor::pos());
    }
}

// 添加右键菜单
void MyFileWg::addActionMenu()
{
    //------------------------- 文件菜单--------------------------
    m_menu = new MyMenu(this);

    // 菜单选项
    m_downloadAction = new QAction("下载", this);
    m_shareAction = new QAction("分享", this);
    m_delAction = new QAction("删除", this);
    m_propertyAction = new QAction("属性", this);

    // 添加对应动作处理函数
    m_menu->addAction(m_downloadAction);
    m_menu->addAction(m_shareAction);
    m_menu->addAction(m_delAction);
    m_menu->addAction(m_propertyAction);

    //------------------------- 空白菜单--------------------------
    m_menuEmpty = new MyMenu( this );
    // 菜单选项
    m_pvAscendingAction = new QAction("按下载量升序", this);
    m_pvDescendingAction = new QAction("按下载量降序", this);
    m_refreshAction = new QAction("刷新", this);
    m_uploadAction = new QAction("上传", this);

    // 定义动作
    m_menuEmpty->addAction(m_pvAscendingAction);
    m_menuEmpty->addAction(m_pvDescendingAction);
    m_menuEmpty->addAction(m_refreshAction);
    m_menuEmpty->addAction(m_uploadAction);

    //-----------------------------菜单槽函数-----------------------
    // 下载
    connect(m_downloadAction, &QAction::triggered, [=]()
    {
        cout << "下载动作";
        //添加需要下载的文件到下载任务列表
        addDownloadFiles();
    });

    // 分享
    connect(m_shareAction, &QAction::triggered, [=]()
    {
        cout << "分享动作";
        dealSelectdFile("分享"); //处理选中的文件
    });

    // 删除
    connect(m_delAction, &QAction::triggered, [=]()
    {
        cout << "删除动作";
        dealSelectdFile("删除"); //处理选中的文件
    });

    // 属性
    connect(m_propertyAction, &QAction::triggered, [=]()
    {
        cout << "属性动作";
        dealSelectdFile("属性"); //处理选中的文件
    });

    // 按下载量升序
    connect(m_pvAscendingAction, &QAction::triggered, [=]()
    {
        cout << "按下载量升序";
        refreshFiles(PvAsc);
    });

    // 按下载量降序
    connect(m_pvDescendingAction, &QAction::triggered, [=]()
    {
        cout << "按下载量降序";
        refreshFiles(PvDesc);
    });

    //刷新
    connect(m_refreshAction, &QAction::triggered, [=]()
    {
        cout << "刷新动作";
        //显示用户的文件列表
        refreshFiles();
    });

    //上传
    connect(m_uploadAction, &QAction::triggered, [=]()
    {
        cout << "上传动作";
        //添加需要上传的文件到上传任务列表
        addUploadFiles();
    });
}


// --------------------------上传文件--------------------------
// 添加上传文件到任务列表
void MyFileWg::addUploadFiles()
{
    emit gotoTransfer(TransferStatus::Uplaod);
    // 获取上传列表
    UploadTask *uploadList = UploadTask::getInstance();
    if(uploadList == NULL)
    {
        cout << "UploadTask::getInstance() == NULL";
        return;
    }

    // 打开文件--上传列表
    QStringList list = QFileDialog::getOpenFileNames();
    // 插入列表
    for (int i = 0; i < list.size(); ++i)
    {
        // -1：文件大于50M
        // -2：已在上传队列
        // -3：打开文件失败
        // -4：获取布局失败
        int ret = uploadList->appendUploadList(list.at(i));
        if(ret == -1){
            QMessageBox::warning(this, "文件过大", "文件上限为50M");
        }
        else if(ret == -2){
            QMessageBox::warning(this, "添加失败", "文件已位于上传列表");
        }
        else if(ret == -3) {
            cout << "打开文件失败";
        }
        else if(ret == -4) {
            cout << "获取布局失败";
        }
    }
}
// 设置MD5 Json 包
QByteArray MyFileWg::setMd5Json(QString user, QString token, QString md5, QString fileName)
{
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("token", token);
    tmp.insert("md5", md5);
    tmp.insert("fileName", fileName);

    /*
    {
        user:xxxx,
        token:xxxx,
        md5:xxx,
        fileName: xxx
    }
    */
    QJsonDocument doc = QJsonDocument::fromVariant(tmp);
    if(doc.isNull())
    {
        cout << "doc.isNull";
        return "";
    }
    return doc.toJson();
}

// 上传文件处理，取出队首，上传完毕取下一个
void MyFileWg::uploadFilesAction()
{
    // 获取上传列表实例
    UploadTask *uploadList = UploadTask::getInstance();
    if(uploadList == NULL)
    {
        cout << "UploadTask::getInstance";
        return;
    }

    // 无上传任务，终止
    if(uploadList->isEmpty())
    {
        return;
    }
    // 有上传任务正在进行，不能上传
    if(uploadList->isUpload())
    {
        return;
    }
    // 获取登录信息
    LoginInfoInstance * loginInfo = LoginInfoInstance::getInstance();

    // 秒传Url
    QNetworkRequest request;
    QString url = QString("http://%1:%2/md5").arg(loginInfo->getIp()).arg(loginInfo->getPort());
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 取第0个任务
    UploadFileInfo *info = uploadList->takeTask();

    // post数据包，秒传
    QByteArray array = setMd5Json(loginInfo->getUser(), loginInfo->getToken(), info->md5, info->fileName);
    QNetworkReply *reply = m_manager->post(request, array);
    if(reply == NULL)
    {
        cout << "reply is NULL";
        return;
    }

    // 信号和槽
    connect(reply, &QNetworkReply::readyRead, [=]()
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            cout << reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray array = reply->readAll();
        // cout << array.data();
        reply->deleteLater();

        /*
        秒传：
        文件已存在：{"code":"005"}
        秒传成功：  {"code":"006"}
        秒传失败：  {"code":"007"}
        token验证失败：{"code":"111"}
        */
        cout << m_cm.getCode(array);
        if(m_cm.getCode(array) == "006")
        {
            // 成功
            m_cm.writeRecord(loginInfo->getUser(), info->fileName, "006");
            // 删除已经完成的任务
            uploadList->dealUploadTask();
        }
        else if(m_cm.getCode(array) == "005")
        {
            // 已存在文件，写文件信息
            m_cm.writeRecord(loginInfo->getUser(), info->fileName, "005");
            // 删除任务
            uploadList->dealUploadTask();
        }
        else if(m_cm.getCode(array) == "007")
        {
            // 秒传失败，真正上传
            uploadFile(info);
        }
        else if(m_cm.getCode(array) == "111")
        {
            QMessageBox::warning(this, "账户异常", "请重新登录");
            emit loginAgainSignal();
            return;
        }
    });
}

// 上传文件，非秒传
void MyFileWg::uploadFile(UploadFileInfo *info)
{
    // 取出上传任务
    /*
    QString md5;        //文件md5码
    QFile *file;        //文件指针
    QString fileName;   //文件名字
    qint64 size;        //文件大小
    QString path;       //文件路径
    bool isUpload;      //是否已经在上传
    DataProgress *dp;   //上传进度控件
    */
    QFile *file = info->file;                                    //文件指针
    QString fileName = info->fileName;              //文件名
    QString md5 = info->md5;                             //文件md5
    qint64 size = info->size;                                 //文件大小
    DataProgress *dp = info->dp;                        //进度条
    QString boundary = m_cm.getBoundary();     // 分割线
    // 获取登录实例
    LoginInfoInstance *login = LoginInfoInstance::getInstance();
    // 构建数据
    /*
    ------WebKitFormBoundary8123sa23wtgewx2d\r\n
    Content-Disposition: form-data; user="abc"; filename="123.jpg"; md5="asww12e"; size=10240\r\n
    Content-Type: application/octet-stream\r\n
    \r\n
    真正的文件内容\r\n
    ------WebKitFormBoundary8123sa23wtgewx2d
    */
    QByteArray data;

    // 分割线
    data.append(boundary);
    data.append("\r\n");

    // 文件信息
    data.append("Content-Disposition: form-data; ");
    data.append(QString("user=\"%1\" ").arg(login->getUser()));     //上传用户
    data.append(QString("filename=\"%1\" ").arg(fileName));            //文件名字
    data.append(QString("md5=\"%1\" ").arg(md5));                           //文件md5码
    data.append(QString("size=%1").arg(size));                                   //文件大小
    data.append("\r\n");

    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");

    // 文件内容
    data.append(file->readAll());
    data.append("\r\n");

    //结束分隔线
    data.append(boundary);

    QNetworkRequest request;
    // http://127.0.0.1:80/upload
    QString url = QString("http://%1:%2/upload").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl(url));

    // 发送post请求
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_manager->post(request, data);
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    // 有可用数据更新，进度条信号
    connect(reply, &QNetworkReply::uploadProgress, [=](qint64 bytesRead, qint64 totalBytes)
    {
        if(totalBytes != 0) //这个条件很重要
        {
            //cout << bytesRead << ", " << totalBytes;
            dp->setProgress(bytesRead, totalBytes); //设置进度条
        }
    });

    // 获取请求的数据
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            cout << reply->errorString();
            reply->deleteLater();  //释放资源
            return;
        }

        QByteArray array = reply->readAll();

        reply->deleteLater();
        /*
        上传文件：
            成功：{"code":"008"}
            失败：{"code":"009"}
        */
        if("008" == m_cm.getCode(array))
        {
            cout << "上传成功";
            m_cm.writeRecord(login->getUser(), info->fileName, "008");
        }
        else if("009" == m_cm.getCode(array))
        {
            cout << "上传失败";
            m_cm.writeRecord(login->getUser(), info->fileName, "009");
        }

        // 获取上传实例
        UploadTask *uploadList = UploadTask::getInstance();
        if(uploadList == NULL)
        {
            cout << "UploadTask::getInstance() == NULL";
            return;
        }

        uploadList->dealUploadTask();   // 删除任务
    });
}



// ---------------------------文件 item 展示---------------------------
// 清空文件列表
void MyFileWg::clearFileList()
{
    int n = m_fileList.size();
    for(int i = 0; i < n; ++i)
    {
        FileInfo *info = m_fileList.takeFirst();
        delete info;
    }
}

// 清空所有 item 项目
void MyFileWg::clearItems()
{
    // QListWidget::count() 统计 item 数目
    int n = ui->listWidget->count();
    for(int i = 0; i < n; ++i)
    {
        // 取出首 listWidget
        QListWidgetItem *item = ui->listWidget->takeItem(0);
        delete item;
    }
}

// 添加上传文件项目 item
void MyFileWg::addUploadItem(QString iconPath, QString name)
{
    ui->listWidget->addItem(new QListWidgetItem(QIcon(iconPath), name));
}
// 文件 item 展示
void MyFileWg::refreshFileItems()
{
    // 清空所有 item
    clearItems();

    // 文件列表不为空，显示列表
    if(m_fileList.isEmpty() == false)
    {
        int n = m_fileList.size();
        for(int i = 0; i < n; ++i)
        {
            FileInfo *info = m_fileList.at(i);
            QListWidgetItem *item = info->item;

            ui->listWidget->addItem(item);
        }
    }

    // 添加 item
    this->addUploadItem();
}


//---------------------显示文件列表------------------------
// desc           降序
// asc             升序
// Normal：普通用户列表，PvAsc：按下载量升序， PvDesc：按下载量降序
// enum Display{Normal, PvAsc, PvDesc};
// 得到服务器json文件
QStringList MyFileWg::getCountStatus(QByteArray json)
{
     QJsonParseError error;
     QStringList list;

     QJsonDocument doc = QJsonDocument::fromJson(json, &error);
     if(error.error == QJsonParseError::NoError)
     {
         // 无错
         // 空或者没有数据
         if(doc.isNull() || doc.isEmpty())
         {
             cout << "doc.isNull() || doc.isEmpty()";
             return list;
         }
         if(doc.isObject())
         {
             QJsonObject obj = doc.object();
             list.append(obj.value("token").toString());
             list.append(obj.value("num").toString());
         }
     }
     else
     {
         cout << "err =" <<error.errorString();
     }
     return list;
}

// 显示用户的文件列表
void MyFileWg::refreshFiles(MyFileWg::Display cmd)
{
     m_userFilesCount = 0;

     QNetworkRequest request;

     LoginInfoInstance *logininfo = LoginInfoInstance::getInstance();

     // 192.168.93.178/myfiles&cmd=count
     QString url = QString("http://%1:%2/myfiles?cmd=count").arg(logininfo->getIp()).arg(logininfo->getPort());
     request.setUrl(QUrl(url));

     // 请求头
     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

     QByteArray data =setGetCountJson(logininfo->getUser(), logininfo->getToken());

     // 发送请求
     QNetworkReply *reply = m_manager->post(request, data);
     if(reply == NULL)
     {
         cout << "reply == NULL";
         return;
     }

     connect(reply, &QNetworkReply::finished, [=]()
     {
         if(reply->error() != QNetworkReply::NoError)
         {
             cout << reply->errorString();
             reply->deleteLater();
             return;
         }

         // 读服务器回复数据
         QByteArray array = reply->readAll();

         // 得到服务器 json 文件
         QStringList list = getCountStatus(array);

         // token 验证失败
         if(list.at(0) == "111")
         {
             QMessageBox::warning(this, "账户异常", "请重新登录");

             // 发送重新登录信号
             emit loginAgainSignal();
             return;
         }

         // 转换为 long
         m_userFilesCount = list.at(1).toLong();
         cout << "userFilesCount = " << m_userFilesCount;

         // 清空文件列表信息
         clearFileList();

         if(m_userFilesCount > 0)
         {
             m_start = 0;   // 0开始
             m_count = 10;  // 一次请求10个

             // 获取文件列表信息
             getUserFilesList(cmd);
         }
         else
         {
             // 没有文件，更新文件 item
             refreshFileItems();
         }
     });
}

// 设置json包
QByteArray MyFileWg::setGetCountJson(QString user, QString token)
{
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("token", token);
    /*json数据如下
    {
        user:xxxx
        token: xxxx
    }
    */
     QJsonDocument doc = QJsonDocument::fromVariant(tmp);
     if(doc.isNull())
     {
         cout << "doc.isNull()";
         return "";
     }

     return doc.toJson();
}

// 设置json包
QByteArray MyFileWg::setFilesListJson(QString user, QString token, int start, int count)
{
    /*{
        "user": "yoyo"
        "token": "xxx"
        "start": 0
        "count": 10
    }*/
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("token", token);
    tmp.insert("start", start);
    tmp.insert("count", count);

    QJsonDocument doc = QJsonDocument::fromVariant(tmp);
    if(doc.isNull())
    {
        cout << "doc.isNull()";
        return "";
    }

    return doc.toJson();
}

// 获取用户文件列表
void MyFileWg::getUserFilesList(MyFileWg::Display cmd)
{
    if(m_userFilesCount <= 0)
    {
        cout << "获取用户文件列表结束";
        refreshFileItems();     // 刷新 item
        return;
    }
    else if(m_count > m_userFilesCount)
    {
        // 请求数量大于用户文件数量
        m_count = m_userFilesCount;
    }

    QNetworkRequest request;    // 请求对象

    // 获取登录实例
    LoginInfoInstance *loginInfo = LoginInfoInstance::getInstance();

    // 获取文件信息    192.168.93.178:80/myfiles&cmd=normal
    // 按下载升序       192.168.93.178:80/myfiles?cmd=pvasc
    // 按下载降序       192.168.93.178:80/myfiles?cmd=pvdesc
    QString flag;
    if(cmd == Normal) {flag = "normal";}
    else if(cmd == PvAsc) {flag = "pvasc";}
    else if(cmd == PvDesc) {flag = "pvdesc";}

    QString url = QString("http://%1:%2/myfiles?cmd=%3").arg(loginInfo->getIp()).arg(loginInfo->getPort()).arg(flag);
    request.setUrl(QUrl(url));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    /*
    {
        "user": "pinna"
        "token": "xxxx"
        "start": 0
        "count": 10
    }
    */
    QByteArray data = setFilesListJson(loginInfo->getUser(), loginInfo->getToken(), m_start, m_count);

    // 改变文件几点位置
    m_start += m_count;
    m_userFilesCount -= m_count;

    // post 请求
    QNetworkReply *reply = m_manager->post(request, data);
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    // 接收数据槽函数
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            cout << reply->errorString();
            reply->deleteLater();
            return;
        }

        // 服务器返回用户数据
        QByteArray array = reply->readAll();
        reply->deleteLater();

        // token 验证失败
        if(m_cm.getCode(array) == "111")
        {
            QMessageBox::warning(this, "账户异常", "请重新登录");
            emit loginAgainSignal();

            return;
        }

        // 不是错误码处理 json 列表
        if(m_cm.getCode(array) != "015")
        {
            // 解析文件列表 json 信息
            getFileJsonInfo(array);     // 解析文件列表 json 信息

            // 继续获得文件列表
            getUserFilesList(cmd);
        }
    });
}

// 解析文件列表json信息，存放在文件列表中
void MyFileWg::getFileJsonInfo(QByteArray data)
{
    /*
    {
    "user": "pinna",
    "md5": "e8ea6031b779ac26c319ddf949ad9d8d",
    "time": "2019-3-23 02:15:43",
    "filename": "test.png",
    "share_status": 0,
    "pv": 0,
    "url": "http://192.168.93.178:80/group1/M00/00/00/wKgfbViy2Z2AJ-FTAaM3Asag3Z0782.png",
    "size": 27473666,
    "type": "png"
    }
    */
    //-- user	文件所属用户
    //-- md5 文件md5
    //-- createtime 文件创建时间
    //-- filename 文件名字
    //-- shared_status 共享状态, 0为没有共享， 1为共享
    //-- pv 文件下载量，默认值为0，下载一次加1
    //-- url 文件url
    //-- size 文件大小, 以字节为单位
    //-- type 文件类型： png

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error == QJsonParseError::NoError)
    {
        if(doc.isNull() || doc.isEmpty())
        {
            cout << "doc.isNull() || doc.isEmpty";
            return;
        }
        if(doc.isObject())
        {
            // 获取 json 对象
            QJsonObject obj = doc.object();
            QJsonArray array = obj.value("files").toArray();

            int size = array.size();
            cout << "size = " << size;

            for(int i = 0; i < size; ++i)
            {
                // 取第一个对象
                QJsonObject tmp = array[i].toObject();
                /*
                    //文件信息
                    struct FileInfo
                    {
                        QString md5;        //文件md5码
                        QString filename;   //文件名字
                        QString user;       //用户
                        QString time;       //上传时间
                        QString url;        //url
                        QString type;       //文件类型
                        qint64 size;        //文件大小
                        int shareStatus;    //是否共享, 1共享， 0不共享
                        int pv;             //下载量
                        QListWidgetItem *item; //list widget 的item
                    };

                    {
                    "user": "pinna",
                    "md5": "e8ea6031b779ac26c319ddf949ad9d8d",
                    "time": "2019-3-23 02:15:43",
                    "filename": "test.png",
                    "share_status": 0,
                    "pv": 0,
                    "url": "http://192.168.93.178:80/group1/M00/00/00/wKgfbViy2Z2AJ-FTAaM3Asag3Z0782.png",
                    "size": 27473666,
                    "type": "png"
                    }
                */
                FileInfo *info = new FileInfo;
                info->user = tmp.value("user").toString(); //用户
                info->md5 = tmp.value("md5").toString(); //文件md5
                info->time = tmp.value("time").toString(); //上传时间
                info->filename = tmp.value("filename").toString(); //文件名字
                info->shareStatus = tmp.value("share_status").toInt(); //共享状态
                info->pv = tmp.value("pv").toInt(); //下载量
                info->url = tmp.value("url").toString(); //url
                info->size = tmp.value("size").toInt(); //文件大小，以字节为单位
                info->type = tmp.value("type").toString();//文件后缀
                QString type = info->type + ".png"; // 类型图片后缀

                // 创建一个 item
                info->item = new QListWidgetItem(QIcon(m_cm.getFileType(type)), info->filename);

                // 添加文件到列表
                m_fileList.append(info);
            }
        }
    }
    else
    {
        cout << "err = " << error.errorString();
    }
}

// -------------------------获取文件属性------------------------
// 获取属性信息
void MyFileWg::getFileProperty(FileInfo *info)
{
    FilePropertyInfo dlg;   // 对话框
    dlg.setInfo(info);

    dlg.exec();  // 模态方式运行
}

// -------------------------下载文件处理----------------------------
// 添加需要下载的文件到下载任务列表
void MyFileWg::addDownloadFiles()
{
    emit gotoTransfer(TransferStatus::Download);
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(item == NULL)
    {
        cout << "item == NULL";
        return;
    }
    //获取下载列表实例
        DownloadTask *p = DownloadTask::getInstance();
        if(p == NULL)
        {
            cout << "DownloadTask::getInstance() == NULL";
            return;
        }

        for(int i = 0; i < m_fileList.size(); ++i)
        {
            if(m_fileList.at(i)->item == item)
            {

                QString filePathName = QFileDialog::getSaveFileName(this, "选择保存文件路径", m_fileList.at(i)->filename );
                if(filePathName.isEmpty())
                {
                    cout << "filePathName.isEmpty()";
                    return;
                }

                /*
                   下载文件：
                        成功：{"code":"009"}
                        失败：{"code":"010"}
                */
                //cout << filePathName;

                //追加任务到下载队列
                //参数：info：下载文件信息， filePathName：文件保存路径
                //成功：0
                //失败：
                //  -1: 下载的文件是否已经在下载队列中
                //  -2: 打开文件失败
                int res = p->appendDownloadList(m_fileList.at(i), filePathName); //追加到下载列表
                if(res == -1)
                {
                    QMessageBox::warning(this, "任务已存在", "任务已经在下载队列中！");
                }
                else if(res == -2) //打开文件失败
                {
                    m_cm.writeRecord(m_fileList.at(i)->user, m_fileList.at(i)->filename, "010"); //下载文件失败，记录
                }

                break;
            }
        }
}
// 下载文件处理，取出下载任务列表的队首任务，下载完后，再取下一个任务
void MyFileWg::downloadFilesAction()
{
    DownloadTask *p = DownloadTask::getInstance();
        if(p == NULL)
        {
            cout << "DownloadTask::getInstance() == NULL";
            return;
        }

        if( p->isEmpty() ) //如果队列为空，说明没有任务
        {
            return;
        }

        if( p->isDownload() ) //当前时间没有任务在下载，才能下载，单任务
        {
            return;
        }

        //看是否是共享文件下载任务，不是才能往下执行, 如果是，则中断程序
        if(p->isShareTask() == true)
        {
            return;
        }

        DownloadInfo *tmp = p->takeTask(); //取下载任务


        QUrl url = tmp->url;
        QFile *file = tmp->file;
        QString md5 = tmp->md5;
        QString user = tmp->user;
        QString filename = tmp->filename;
        DataProgress *dp = tmp->dp;

        //发送get请求
        QNetworkReply * reply = m_manager->get( QNetworkRequest(url) );
        if(reply == NULL)
        {
            p->dealDownloadTask(); //删除任务
            cout << "get err";
            return;
        }

        connect(reply, &QNetworkReply::finished, [=]()
        {
            cout << "下载完成";
            reply->deleteLater();

            p->dealDownloadTask();//删除下载任务

            m_cm.writeRecord(user, filename, "010"); //下载文件成功，记录

            dealFilePv(md5, filename); //下载文件pv字段处理
        });

        connect(reply, &QNetworkReply::readyRead, [=]()
        {
            //如果文件存在，则写入文件
            if (file != NULL)
            {
                file->write(reply->readAll());
            }
        });

        //有可用数据更新时
        connect(reply, &QNetworkReply::downloadProgress, [=](qint64 bytesRead, qint64 totalBytes)
        {
            dp->setProgress(bytesRead, totalBytes);//设置进度
        });
}

// 下载文件标志 pv 处理
void MyFileWg::dealFilePv(QString md5, QString filename)
{
    QNetworkRequest request; //请求对象

    //获取登陆信息实例
    LoginInfoInstance *login = LoginInfoInstance::getInstance(); //获取单例

    //127.0.0.1:80/dealfile?cmd=pv
    QString url = QString("http://%1:%2/dealfile?cmd=pv").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); //设置url

    //qt默认的请求头
    //request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    /*
    {
        "user": "pinna",
        "token": "xxx",
        "md5": "xxx",
        "filename": "xxx"
    }
    */
    QByteArray data = setDealFileJson( login->getUser(), login->getToken(), md5, filename); //设置json包

    //发送post请求
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) //有错误
        {
            cout << reply->errorString();
            reply->deleteLater(); //释放资源
            return;
        }

        //服务器返回用户的数据
        QByteArray array = reply->readAll();

        reply->deleteLater();

        /*
            下载文件pv字段处理
                成功：{"code":"016"}
                失败：{"code":"017"}
            */
        if("016" == m_cm.getCode(array) ) //common.h
        {
            //该文件pv字段+1
            for(int i = 0; i < m_fileList.size(); ++i)
            {
                FileInfo *info = m_fileList.at(i);
                if( info->md5 == md5 && info->filename == filename)
                {
                    int pv = info->pv;
                    info->pv = pv+1;

                    break;
                }
            }
        }
        else
        {
            cout << "下载文件pv字段处理失败";
        }

    });
}


// ------------------------分享/删除文件------------------------
// 处理选中的文件
void MyFileWg::dealSelectdFile(QString cmd)
{
    //获取当前选中的item
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(item == NULL)
    {
        return;
    }

    //查找文件列表匹配的元素
    for(int i = 0; i < m_fileList.size(); ++i)
    {
        if(m_fileList.at(i)->item == item)
        {
            if(cmd == "分享")
            {
                shareFile( m_fileList.at(i) ); //分享某个文件
            }
            else if(cmd == "删除")
            {
                delFile( m_fileList.at(i) ); //删除某个文件
            }
            else if(cmd == "属性")
            {
                getFileProperty( m_fileList.at(i) ); //获取属性信息
            }
            break;
        }
    }
}

// 设置删除 json 包
QByteArray MyFileWg::setDealFileJson(QString user, QString token, QString md5, QString filename)
{
    /*
       {
           "user": "pinna",
           "token": "xxxx",
           "md5": "xxx",
           "filename": "xxx"
       }
       */
       QMap<QString, QVariant> tmp;
       tmp.insert("user", user);
       tmp.insert("token", token);
       tmp.insert("md5", md5);
       tmp.insert("filename", filename);

       QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
       if ( jsonDocument.isNull() )
       {
           cout << " jsonDocument.isNull() ";
           return "";
       }
       //cout << jsonDocument.toJson().data();

       return jsonDocument.toJson();
}

// 分享文件
void MyFileWg::shareFile(FileInfo *info)
{
    if(info->shareStatus == 1) //已经分享，不能再分享
        {
            QMessageBox::warning(this, "此文件已经分享", "此文件已经分享!");
            return;
        }

        QNetworkRequest request; //请求对象

        //获取登陆信息实例
        LoginInfoInstance *login = LoginInfoInstance::getInstance(); //获取单例

        //127.0.0.1:80/dealfile?cmd=share
        QString url = QString("http://%1:%2/dealfile?cmd=share").arg(login->getIp()).arg(login->getPort());
        request.setUrl(QUrl( url )); //设置url

        //qt默认的请求头
        //request.setRawHeader("Content-Type","text/html");
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

        /*
        {
            "user": "pinna",
            "token": "xxxx",
            "md5": "xxx",
            "filename": "xxx"
        }
        */
        QByteArray data = setDealFileJson( login->getUser(),  login->getToken(), info->md5, info->filename);

        //发送post请求
        QNetworkReply * reply = m_manager->post( request, data );
        if(reply == NULL)
        {
            cout << "reply == NULL";
            return;
        }

        connect(reply, &QNetworkReply::finished, [=]()
        {
            if (reply->error() != QNetworkReply::NoError) //有错误
            {
                cout << reply->errorString();
                reply->deleteLater(); //释放资源
                return;
            }

            //服务器返回用户的数据
            QByteArray array = reply->readAll();

            reply->deleteLater();

            /*
                分享文件：
                    成功：{"code":"010"}
                    失败：{"code":"011"}
                    别人已经分享此文件：{"code", "012"}

                    token验证失败：{"code":"111"}

                */
            if("010" == m_cm.getCode(array) ) //common.h
            {
                //修改文件列表的属性信息
                info->shareStatus = 1; //设置此文件为已分享
                QMessageBox::information(this, "分享成功", QString("[%1] 分享成功!!!").arg(info->filename));
            }
            else if("011" == m_cm.getCode(array))
            {
                QMessageBox::warning(this, "分享失败", QString("[%1] 分享失败!!!").arg(info->filename));
            }
            else if("012" == m_cm.getCode(array))
            {
                QMessageBox::warning(this, "分享失败", QString("别人已分享过: [%1]").arg(info->filename));
            }
            else if("111" == m_cm.getCode(array))   //token验证失败
            {
                QMessageBox::warning(this, "账户异常", "请重新登陆！");
                emit loginAgainSignal(); //发送重新登陆信号

                return;
            }
        });
}

// 删除文件
void MyFileWg::delFile(FileInfo *info)
{
    QNetworkRequest request; //请求对象

    //获取登陆信息实例
    LoginInfoInstance *login = LoginInfoInstance::getInstance(); //获取单例

    //127.0.0.1:80/dealfile?cmd=del
    QString url = QString("http://%1:%2/dealfile?cmd=del").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); //设置url

    //qt默认的请求头
    //request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    /*
    {
        "user": "pinna",
        "token": "xxxx",
        "md5": "xxx",
        "filename": "xxx"
    }
    */
    QByteArray data = setDealFileJson( login->getUser(),  login->getToken(), info->md5, info->filename);

    //发送post请求
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) //有错误
        {
            cout << reply->errorString();
            reply->deleteLater(); //释放资源
            return;
        }

        //服务器返回用户的数据
        QByteArray array = reply->readAll();

        reply->deleteLater();

        /*
            删除文件：
                成功：{"code":"013"}
                失败：{"code":"014"}
            */
        if("013" == m_cm.getCode(array) ) //common.h
        {
            QMessageBox::information(this, "文件删除成功", QString("[%1] 删除成功!").arg(info->filename));
            //从文件列表中移除该文件，移除列表视图中此item
            for(int i = 0; i < m_fileList.size(); ++i)
            {
                if( m_fileList.at(i) == info)
                {
                    QListWidgetItem *item = info->item;
                    //从列表视图移除此item
                    ui->listWidget->removeItemWidget(item);
                    delete item;

                    m_fileList.removeAt(i);
                    delete info;
                    break;
                }
            }
        }
        else if("014" == m_cm.getCode(array))
        {
            QMessageBox::information(this, "文件删除失败", QString("[%1] 删除失败!").arg(info->filename));
        }
        else if("111" == m_cm.getCode(array)) //token验证失败
        {
            QMessageBox::warning(this, "账户异常", "请重新登陆！");
            emit loginAgainSignal(); //发送重新登陆信号

            return; //中断
        }
    });
}



//清除上传下载任务
void MyFileWg::clearAllTask()
{
    // 获取上传列表实例
    UploadTask *uploadList = UploadTask::getInstance();
    if(uploadList == NULL)
    {
        cout << "UploadTask::getInstance() == NULL";
        return;
    }
    // 清空
    uploadList->clearList();

    // 获取下载列表
    DownloadTask *p = DownloadTask::getInstance();
    // 清空
    if(p == NULL)
    {
        cout << "DownloadTask::getInstance() == NULL";
        return;
    }

    p->clearList();
}

// 定时检查处理任务队列中的任务
void MyFileWg::checkTaskList()
{
    // 检查上传任务队列有无任务
    connect(&m_uploadFileTimer, &QTimer::timeout, [=]()
    {
        // 取出队首列表任务上传
        uploadFilesAction();
    });

    // 定时器 500 毫秒
    m_uploadFileTimer.start(500);

    // 检查队列有无任务需要下载
    // 定时检查下载队列是否有任务需要下载
    connect(&m_downloadTimer, &QTimer::timeout, [=]()
    {
        // 上传文件处理，取出上传任务列表的队首任务，上传完后，再取下一个任务
        downloadFilesAction();
    });

    // 启动定时器，500毫秒间隔
    // 每个500毫秒，检测下载任务，每一次只能下载一个文件
    m_downloadTimer.start(500);
}











