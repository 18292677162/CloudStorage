#include "login.h"
#include "ui_login.h"
#include <QPainter>
#include <QMessageBox>
#include  <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // 去掉边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    // 设置除ui设置外所有字体
    this->setFont(QFont("宋体", 11, QFont::Normal, false));

    // titlewidget信号处理
    connect(ui->title_wg, &TitleWg::showSetWg, this, [=]()
    {
        // 切换到设置
        ui->stackedWidget->setCurrentWidget(ui->set_page);
        // 清空密码
        ui->passwd_login->clear();
    });

    connect(ui->title_wg, &TitleWg::closeWindow , this, [=]()
    {
        // 判断当前页面 or 首页面
        if(ui->stackedWidget->currentWidget() == ui->set_page)
        {
            // 切换到登录
            ui->stackedWidget->setCurrentWidget(ui->login_page);
        }
        else if(ui->stackedWidget->currentWidget() == ui->reg_page)
        {
            // 切换到登录
            ui->stackedWidget->setCurrentWidget(ui->login_page);
            // 清空数据
            ui->name_reg->clear();
            ui->id_reg ->clear();
            ui->passwd__reg->clear();
            ui->re_passwd_reg->clear();
            ui->phone_reg->clear();
            ui->email_reg->clear();
        }
        else
        {
            this->close();
        }
    });

    // 注册
    connect(ui->sign_up, &QToolButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->reg_page);
        // 设置焦点（默认选取）
        ui->name_reg->setFocus();
    });
}

Login::~Login()
{
    delete ui;
}

void Login::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // 绘制背景
    QPainter p(this);
    QPixmap pixmap(":/image/login_bk.jpg");
    p.drawPixmap(0, 0, this->width(), this->height(), pixmap);
}

// 用户注册
void Login::on_signup_button_2_clicked()
{
    // 取数据
    QString ip = ui->ip_set->text();
    QString port = ui->port_set->text();

    QString name = ui->name_reg->text();
    QString nick = ui->id_reg->text();
    QString pwd = ui->passwd__reg->text();
    QString phone = ui->phone_reg->text();
    QString email = ui->email_reg->text();
    // 正则表达式校验
    // 注册信息 to json
    QByteArray postData = getRegJson(name, nick, pwd, phone, email);

    // http 请求协议类  Common全局对象
    QNetworkAccessManager * manager = Common::getNetManager();
    // http 头
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, postData.size());
    // url
    QString url = QString("http://%1:%2/reg").arg(ip).arg(port);
    request.setUrl(QUrl(url));
    QNetworkReply* reply = manager->post(request, postData);
    // 接收 server 回复
    connect(reply, &QNetworkReply::readyRead, [=]()
    {
        // 读取数据
        QByteArray jsonRes = reply->readAll();
        /*
        QJsonDocument docRes = QJsonDocument::fromJson(jsonRes);
        // 取回复
        QJsonObject objRes =  docRes.object();
        // 取回复子对象数据
        QString status = objRes.value("code").toString();
        */
        QString status = m_cm.getCode(jsonRes);

        if(status == "002")
        {
            // success
            // 当前注册信息填入登录框
            ui->name_login->setText(name);
            ui->passwd_login->setText(pwd);
            // 清除注册信息
            ui->name_reg->clear();
            ui->id_reg ->clear();
            ui->passwd__reg->clear();
            ui->re_passwd_reg->clear();
            ui->phone_reg->clear();
            ui->email_reg->clear();
            // 跳转到登录界面
            ui->stackedWidget->setCurrentWidget(ui->login_page);
            qDebug() << "success";
        }
        else if(status == "003")
        {
            // 已存在
            QMessageBox::warning(this, "警告", "当前注册用户已存在!");
            qDebug() << "已存在";
        }
        else if(status == "004")
        {
            // fail
            QMessageBox::warning(this, "警告", "注册失败!");
            qDebug() << "fail";
        }
    });
}

// 服务器设置
void Login::on_toolButton_4_clicked()
{
    // 获取控件数据
    QString ip = ui->ip_set->text();
    QString port = ui->port_set->text();
    // 数据校验 -- 正则表达式
    QRegExp exp(IP_REG);
    if(!exp.exactMatch(ip))
    {
        QMessageBox::warning(this, "警告", "IP格式输入不正确");
        ui->ip_set->clear();
        // 设置焦点
        ui->ip_set->setFocus();
        return;
    }
    // 端口校验 -- regexp
    QRegExp exp1(PORT_REG);
    if(!exp1.exactMatch(port))
    {
        QMessageBox::critical(this, "警告", "端口格式输入不正确");
        ui->port_set->clear();
        // 焦点
        ui->port_set->setFocus();
    }
    // 跳转到登陆界面
    ui->stackedWidget->setCurrentWidget(ui->login_page);
    // 将配置信息写入配置文件中
    m_cm.writeWebInfo(ip, port);
    return;
}

// 保存配置文件信息
/*
void Login::saveWebInfo(QString ip, QString port, QString type_path)
{
    // 读文件
    QFile file(type_path);
    // 文件是否存在
    bool bl = file.open(QIODevice::ReadOnly);
    if(bl == false)
    {
        // 创建 json 对象
        QJsonObject newobj;

        QJsonObject newlogin;
        newlogin.insert("pwd", "");
        newlogin.insert("remember", "");
        newlogin.insert("user", "");

        QJsonObject newtype_path;
        newtype_path.insert("path", "");

        QJsonObject newweb_server;
        newweb_server.insert("ip", "");
        newweb_server.insert("port", "");

        newobj.insert("login", QJsonValue(newlogin));
        newobj.insert("type_path", QJsonValue(newtype_path));
        newobj.insert("web_server", QJsonValue(newweb_server));

        // 写入文件
        QJsonDocument newdoc(newobj);
        // 将 json 对象转换为字符串
        QByteArray newFile = newdoc.toJson();
        // 写新文件
        file.open(QIODevice::WriteOnly);
        file.write(newFile);
        file.close();
        return;
    }
    // 存在
    QByteArray data = file.readAll();
    // 读配置文件信息
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject())
    {
        return;
    }
    // 取 login
    QJsonObject obj =  doc.object();
    QJsonObject loginobj = obj.value("login").toObject();
    // 取 login 子对象数据
    QString pwd = loginobj.value("pwd").toString();
    QString remember = loginobj.value("remember").toString();
    QString user = loginobj.value("user").toString();
    // 存储
    QMap<QString, QVariant> logininfo;
    logininfo.insert("pwd", pwd);
    logininfo.insert("remember", remember);
    logininfo.insert("user", user);

    // 取 path 子对象数据
    QJsonObject pathobj = obj.value("login").toObject();
    QString path = pathobj.value("path").toString();
    // 存储
    QMap<QString, QVariant> pathinfo;
    pathinfo.insert("path", path);

    // 存储 web 信息
    QMap<QString, QVariant> webinfo;
    webinfo.insert("ip", QVariant(ip));
    webinfo.insert("port", QVariant(port));

    QMap<QString, QVariant> info;
    info.insert("login", logininfo);
    info.insert("type_path", pathinfo);
    info.insert("web_server", webinfo);

    doc = QJsonDocument::fromVariant(info);
    // json -> 字符串 保存
    data = doc.toJson();
    // 写入文件
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}
*/


// 编辑注册 json data
QByteArray Login::getRegJson(QString user, QString nick, QString pwd, QString phone, QString email)
{
    QJsonObject regObj;
    regObj.insert("userName", user);
    regObj.insert("nickName", nick);
    regObj.insert("firstPwd", pwd);
    regObj.insert("phone", phone);
    regObj.insert("email", email);

    // obj -> jsondoc
    QJsonDocument regInfo(regObj);
    return regInfo.toJson();
}

// 用户登录
void Login::on_signin_button_clicked()
{
    // 取数据
    QString user = ui->name_login->text();
    QString passwd = ui->passwd_login->text();
    QString ip = ui->ip_set->text();
    QString port = ui->port_set->text();
    // 数据校验
    // 是否保存密码
    bool remember = ui->remember_check->isChecked();
    // 将文件保存到配置文件
    m_cm.writeLoginInfo(user, passwd, remember);
    // 登录的 json 数据包
}
