#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include "UserManager.h"

class ServerManager : public QObject
{
    Q_OBJECT

public:
    explicit ServerManager(QObject *parent = nullptr);
    ~ServerManager();

    // 启动服务器
    bool startServer(quint16 port = 8080);

    // 停止服务器
    void stopServer();

private slots:
    // 新客户端连接
    void onNewConnection();

    // 客户端断开连接
    void onClientDisconnected();

    // 接收客户端数据
    void onReadyRead();

    // 用户管理器信号处理
    void onLoginSuccess(const UserInfo& userInfo);
    void onLoginFailed(LoginResult result);
    void onRegisterSuccess();
    void onRegisterFailed(RegisterResult result);

private:
    QTcpServer* m_tcpServer;
    UserManager* m_userManager;
    QMap<QTcpSocket*, QString> m_clientMap; // 客户端连接映射

    // 处理客户端请求
    void handleClientRequest(QTcpSocket* client, const QJsonObject& request);

    // 发送响应给客户端
    void sendResponse(QTcpSocket* client, const QJsonObject& response);

    // 处理登录请求
    void handleLoginRequest(QTcpSocket* client, const QJsonObject& data);

    // 处理注册请求
    void handleRegisterRequest(QTcpSocket* client, const QJsonObject& data);
};

#endif // SERVERMANAGER_H
