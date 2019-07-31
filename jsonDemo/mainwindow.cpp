#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>

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
}

MainWindow::~MainWindow()
{
    delete ui;
}
