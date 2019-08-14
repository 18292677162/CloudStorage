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
#include "common/des.h"
#include "common/logininfoinstance.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // 去掉边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    // 设置除ui设置外所有字体
    this->setFont(QFont("宋体", 11, QFont::Normal, false));

    // 指针
    m_mainWin = new MainWindow;

    // 窗口图标
    this->setWindowIcon(QIcon(":/image/logo.ico"));
    m_mainWin->setWindowIcon(QIcon(":/image/logo.ico"));

    // 密码打码
    ui->passwd_reg->setEchoMode(QLineEdit::Password);
    ui->passwd_login->setEchoMode(QLineEdit::Password);
    ui->re_passwd_reg->setEchoMode(QLineEdit::Password);

    // titlewidget信号处理
    // 切换到设置
    connect(ui->title_wg, &TitleWg::showSetWg, this, [=]()
    {
        // 切换到设置
        ui->stackedWidget->setCurrentWidget(ui->set_page);
    });

    // 关闭
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
            ui->passwd_reg->clear();
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

    // 切换用户
    connect(m_mainWin, &MainWindow::changeUser, [=]()
    {
        m_mainWin->hide();
        ui->passwd_login->clear();
        this->show();
    });

    // 初始化信息
    readCfg();

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
    QString firstPwd = ui->passwd_reg->text();
    QString secondPwd = ui->passwd_reg->text();
    QString phone = ui->phone_reg->text();
    QString email = ui->email_reg->text();
    // 正则表达式校验
    QRegExp regexp(USER_REG);
    if(!regexp.exactMatch(name))
    {
        QMessageBox::warning(this, "警告", "用户名格式不正确");
        ui->name_reg->clear();
        ui->name_reg->setFocus();
        return;
    }
    if(!regexp.exactMatch(nick))
    {
        QMessageBox::warning(this, "警告", "昵称格式不正确");
        ui->id_reg->clear();
        ui->id_reg->setFocus();
        return;
    }
    regexp.setPattern(PASSWD_REG);
    if(!regexp.exactMatch(firstPwd))
    {
        QMessageBox::warning(this, "警告", "密码格式不正确");
        ui->passwd_reg->clear();
        ui->passwd_reg->setFocus();
        return;
    }
    if(firstPwd != secondPwd)
    {
        QMessageBox::warning(this, "警告", "两次密码输入不一致，请重新输入");
        ui->re_passwd_reg->clear();
        ui->re_passwd_reg->setFocus();
        return;
    }
    regexp.setPattern(PHONE_REG);
    if(!regexp.exactMatch(phone))
    {
        QMessageBox::warning(this, "警告", "手机号码格式不正确");
        ui->phone_reg->clear();
        ui->phone_reg->setFocus();
        return;
    }
    regexp.setPattern(EMAIL_REG);
    if(!regexp.exactMatch(email))
    {
        QMessageBox::warning(this, "警告", "邮箱格式不正确");
        ui->email_reg->clear();
        ui->email_reg->setFocus();
        return;
    }
    // 注册信息 to json
    QByteArray postData = setRegJson(name, nick, firstPwd, phone, email);

    // http 请求协议类  Common单例模式全局对象
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
            ui->passwd_login->setText(firstPwd);
            // 清除注册信息
            ui->name_reg->clear();
            ui->id_reg ->clear();
            ui->passwd_reg->clear();
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
        // 释放资源
        delete reply;
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
QByteArray Login::setRegJson(QString user, QString nick, QString pwd, QString phone, QString email)
{
    /*
        {
            userName:xxxx,
            nickName:xxx,
            firstPwd:xxx,
            phone:xxx,
            email:xxx
        }
    */
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
    QRegExp regexp(USER_REG);
    if(!regexp.exactMatch(user))
    {
        QMessageBox::warning(this, "警告", "用户名格式不正确");
        ui->name_login->clear();
        ui->name_login->setFocus();
        return;
    }
    regexp.setPattern(PASSWD_REG);
    if(!regexp.exactMatch(user))
    {
        QMessageBox::warning(this, "登录失败", "用户名或密码不正确！");
        ui->passwd_login->clear();
        ui->passwd_login->setFocus();
        return;
    }
    // 是否保存密码
    bool remember = ui->remember_check->isChecked();
    // 将文件保存到配置文件
    m_cm.writeLoginInfo(user, passwd, remember);
    // 登录的 json 数据包  MD5加密
    QByteArray info = setLoginJson(user, m_cm.getStrMd5(passwd));
    QString url = QString("http://%1:%2/login").arg(ip).arg(port);

    // 请求 http 数据
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // 请求头Header  ContentType/ContentLength
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(info.size()));

    // 发送 post 请求
    QNetworkAccessManager *manger = m_cm.getNetManager();
    QNetworkReply *reply = manger->post(request, info);
    cout << "post url:" << url << "post data:" << info;

    // 接收服务器回复 finished/readyRead  信号
    connect(reply, &QNetworkReply::finished, [=]()
    {
        // 出错
        if(reply->error() != QNetworkReply::NoError)
        {
            cout << reply->errorString();
            // 释放资源
            reply->deleteLater();  //温柔退出
            return;
        }

        // 成功
        // 读服务器回复数据
        QByteArray json = reply->readAll();
        /*
            成功：{"code":"000"}
            失败：{"code":"001"}
        */
        cout << "server return code:" << json;
        QStringList tmpList = getLoginStatus(json);
        if(tmpList.at(0) == "000")
        {
            cout << "登录成功";

            // 设置登陆信息，显示文件列表界面需要使用这些信息
            LoginInfoInstance *loginInfo = LoginInfoInstance::getInstance(); //获取单例
            loginInfo->setLoginInfo(user, ip, port, tmpList.at(1));
            cout << loginInfo->getUser().toUtf8().data() << ", " << loginInfo->getIp() << ", " << loginInfo->getPort() << tmpList.at(1);

            // 隐藏当前窗口
            this->hide();
            // 跳转
            m_mainWin->showMainWin();
        }
        // 失败
        else
        {
            QMessageBox::warning(this, "登录失败", "用户名或密码不正确！");
        }
        // 释放资源
        reply->deleteLater();
    });
}

QByteArray Login::setLoginJson(QString user, QString pwd)
{
    /*
        {
            user:xxxx,
            pwd:xxx
        }
    */
    QMap<QString, QVariant> login;
    login.insert("user", user);
    login.insert("user", pwd);
    QJsonDocument doc = QJsonDocument::fromVariant(login);
    if(doc.isEmpty())
    {
        cout << "Login doc.isNULL()";
        return "";
    }
    return doc.toJson();
}

QStringList Login::getLoginStatus(QByteArray json)
{
    QJsonParseError m_error;
    QStringList list;

    // json to JSONdoc
    QJsonDocument doc = QJsonDocument::fromJson(json, &m_error);
    if(m_error.error == QJsonParseError::NoError)
    {
        if(doc.isNull() || doc.isEmpty())
        {
            cout << "doc.isNull() || doc.isEmpty()";
            return list;
        }
        if(doc.isObject())
        {
            // 解析大对象
            QJsonObject obj = doc.object();
            cout << "服务器回复数据:" << obj;
            // 状态码
            list.append(obj.value("code").toString());
            // Token
            list.append(obj.value("token").toString());
        }
    }
    else
    {
        cout << "err:" << m_error.errorString();
    }
    return list;
}

// 读取配置信息，设置默认登录状态，默认设置信息
void Login::readCfg()
{
    QString user = m_cm.getCfgValue("login", "user");
    QString pwd = m_cm.getCfgValue("login", "pwd");
    QString remeber = m_cm.getCfgValue("login", "remember");
    int ret = 0;

    if(remeber == "yes")//记住密码
    {
        //密码解密
        unsigned char encPwd[512] = {0};
        int encPwdLen = 0;
        //toLocal8Bit(), 转换为本地字符集，默认windows则为gbk编码，linux为utf-8编码
        QByteArray tmp = QByteArray::fromBase64( pwd.toLocal8Bit());
        ret = DesDec( (unsigned char *)tmp.data(), tmp.size(), encPwd, &encPwdLen);
        if(ret != 0)
        {
            cout << "DesDec";
            return;
        }

    #ifdef _WIN32 //如果是windows平台
        //fromLocal8Bit(), 本地字符集转换为utf-8
        ui->passwd_login->setText( QString::fromLocal8Bit( (const char *)encPwd, encPwdLen ) );
    #else //其它平台
        ui->passwd_login->setText( (const char *)encPwd );
    #endif

        ui->remember_check->setChecked(true);

    }
    else //没有记住密码
    {
        ui->passwd_login->setText("");
        ui->remember_check->setChecked(false);
    }

    //用户解密
    unsigned char encUsr[512] = {0};
    int encUsrLen = 0;
    //toLocal8Bit(), 转换为本地字符集，如果windows则为gbk编码，如果linux则为utf-8编码
    QByteArray tmp = QByteArray::fromBase64( user.toLocal8Bit());
    ret = DesDec( (unsigned char *)tmp.data(), tmp.size(), encUsr, &encUsrLen);
    if(ret != 0)
    {
        cout << "DesDec";
        return;
    }

    #ifdef _WIN32 //如果是windows平台
        //fromLocal8Bit(), 本地字符集转换为utf-8
        ui->name_login->setText( QString::fromLocal8Bit( (const char *)encUsr, encUsrLen ) );
    #else //其它平台
        ui->name_login->setText( (const char *)encUsr );
    #endif

    QString ip = m_cm.getCfgValue("web_server", "ip");
    QString port = m_cm.getCfgValue("web_server", "port");
    ui->ip_set->setText(ip);
    ui->port_set->setText(port);
}
