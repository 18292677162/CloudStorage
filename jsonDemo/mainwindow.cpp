#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QCryptographicHash>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建 json 对象
    QJsonObject obj;

    QJsonObject sub;
    sub.insert("ip", QJsonValue("192.168.93.178"));
    sub.insert("port", QJsonValue("8001"));

    obj.insert("server", QJsonValue(sub));

    // 写入文件
    QJsonDocument doc(obj);
    // 将 json 对象转换为字符串
    QByteArray data = doc.toJson();
    // 写入文件
    QFile file("F:\\QtProject\\jsonDemo\\Qtemp.json");
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    // 读取 json 文件
    QFile file1("F:\\QtProject\\jsonDemo\\Qtemp.json");
    file1.open(QIODevice::ReadOnly);
    QByteArray data1 = file1.readAll();
    file1.close();

    // 使用 json 对象加载 json 字符串
    QJsonDocument doc1 = QJsonDocument::fromJson(data1);
    // 判断是 array or object
    if(doc1.isObject())
    {
        QJsonObject obj1 = doc.object();
        QJsonValue value = obj.value("server");
        if(value.isObject())
        {
            // 转换为对象
            QJsonObject subobj = value.toObject();
            // 取值
            QString ip = subobj.value("ip").toString();
            QString port = subobj.value("port").toString();
            qDebug() << ip << port;
        }
    }

    // 创建 manager 对象，做 post, get，整个项目只需一个
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    // 初始化一个 QNetworkRequest 类
    QNetworkRequest res;

    // 设置 http headers 伪装浏览器
    res.setHeader(QNetworkRequest::UserAgentHeader,
                  "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36");
    res.setUrl(QUrl("http://www.baidu.com:80"));
    // 接收服务器数据
    QNetworkReply *reply = manager->post(res, "");

    // 读服务器回复数据
    connect(reply, &QNetworkReply::readyRead, this, [=]()
    {
        // 读取数据
        QVariant str = reply->header(QNetworkRequest::LocationHeader);
        // 数据 - 不包括 Header
        QByteArray data2 = reply->readAll();
        qDebug() << data2;
        qDebug() << str.toString();
    });

    // MD5
    QByteArray data3 = "hello,world";
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData("你好");

    // hash 计算
    QByteArray arry = hash.result();
    qDebug() << arry;
    arry = arry.toHex();
    qDebug() << arry;

    // MD5
    QByteArray arry1 = QCryptographicHash::hash("hello, world", QCryptographicHash::Md5);
    qDebug() << arry1.toHex();

    // Base64
    QByteArray base = "hello,world";
    base = base.toBase64();
    qDebug() << base.data();
    // 解码
    base = QByteArray::fromBase64(base);
    qDebug() << base.data();
}

MainWindow::~MainWindow()
{
    delete ui;
}
