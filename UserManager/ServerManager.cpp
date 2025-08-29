#include "ServerManager.h"
#include <QDebug>
#include <QJsonParseError>

ServerManager::ServerManager(QObject *parent)
    : QObject(parent)
    , m_tcpServer(new QTcpServer(this))
    , m_userManager(new UserManager(this))
{
    // 连接用户管理器的信号
    connect(m_userManager, &UserManager::loginSuccess, this, &ServerManager::onLoginSuccess);
    connect(m_userManager, &UserManager::loginFailed, this, &ServerManager::onLoginFailed);
    connect(m_userManager, &UserManager::registerSuccess, this, &ServerManager::onRegisterSuccess);
    connect(m_userManager, &UserManager::registerFailed, this, &ServerManager::onRegisterFailed);

    // 连接服务器信号
    connect(m_tcpServer, &QTcpServer::newConnection, this, &ServerManager::onNewConnection);
}

ServerManager::~ServerManager()
{
    stopServer();
}

bool ServerManager::startServer(quint16 port)
{
    if (m_tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "服务器启动成功，监听端口:" << port;
        return true;
    } else {
        qDebug() << "服务器启动失败:" << m_tcpServer->errorString();
        return false;
    }
}

void ServerManager::stopServer()
{
    if (m_tcpServer->isListening()) {
        m_tcpServer->close();
        qDebug() << "服务器已停止";
    }
}

void ServerManager::onNewConnection()
{
    QTcpSocket* client = m_tcpServer->nextPendingConnection();

    connect(client, &QTcpSocket::readyRead, this, &ServerManager::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &ServerManager::onClientDisconnected);

    m_clientMap.insert(client, QString("Client_%1").arg(client->socketDescriptor()));

    qDebug() << "新客户端连接:" << client->peerAddress().toString() << ":" << client->peerPort();
}

void ServerManager::onClientDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QString clientName = m_clientMap.value(client);
        m_clientMap.remove(client);
        qDebug() << "客户端断开连接:" << clientName;
        client->deleteLater();
    }
}

void ServerManager::onReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QByteArray data = client->readAll();

    // 解析JSON数据
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << parseError.errorString();
        return;
    }

    QJsonObject request = doc.object();
    handleClientRequest(client, request);
}

void ServerManager::handleClientRequest(QTcpSocket* client, const QJsonObject& request)
{
    QString action = request["action"].toString();
    QJsonObject data = request["data"].toObject();

    if (action == "login") {
        handleLoginRequest(client, data);
    } else if (action == "register") {
        handleRegisterRequest(client, data);
    } else {
        qDebug() << "未知请求类型:" << action;
    }
}

void ServerManager::handleLoginRequest(QTcpSocket* client, const QJsonObject& data)
{
    QString username = data["username"].toString();
    QString password = data["password"].toString();

    qDebug() << "处理登录请求:" << username;

    // 存储当前请求的客户端，以便在回调中使用
    m_clientMap[client] = username;

    // 调用用户管理器进行登录验证
    m_userManager->userLogin(username, password);
}

void ServerManager::handleRegisterRequest(QTcpSocket* client, const QJsonObject& data)
{
    QString username = data["username"].toString();
    QString password = data["password"].toString();
    QString identity = data["identity"].toString();
    QString gender = data["gender"].toString();
    int age = data["age"].toInt();
    QString phone = data["phone"].toString();

    qDebug() << "处理注册请求:" << username;

    // 存储当前请求的客户端
    m_clientMap[client] = username;

    // 调用用户管理器进行注册
    m_userManager->userRegister(username, password, identity, gender, age, phone);
}

void ServerManager::sendResponse(QTcpSocket* client, const QJsonObject& response)
{
    QJsonDocument doc(response);
    client->write(doc.toJson());
    client->flush();
}

void ServerManager::onLoginSuccess(const UserInfo& userInfo)
{
    // 找到对应的客户端
    QTcpSocket* client = nullptr;
    for (auto it = m_clientMap.begin(); it != m_clientMap.end(); ++it) {
        if (it.value() == userInfo.username) {
            client = it.key();
            break;
        }
    }

    if (client) {
        QJsonObject response;
        response["action"] = "login_response";
        response["success"] = true;
        response["message"] = "登录成功";
        response["userType"] = (userInfo.identity == PATIENT ? "patient" : "doctor");

        sendResponse(client, response);
    }
}

void ServerManager::onLoginFailed(LoginResult result)
{
    // 这里可以根据需要实现更详细的错误处理
    qDebug() << "登录失败，错误代码:" << result;
}

void ServerManager::onRegisterSuccess()
{
    // 找到最近注册的客户端并发送成功响应
    qDebug() << "注册成功";
}

void ServerManager::onRegisterFailed(RegisterResult result)
{
    // 处理注册失败
    qDebug() << "注册失败，错误代码:" << result;
}
