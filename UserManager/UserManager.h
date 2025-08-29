#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "UserInfo.h"
#include "DatabaseManager.h"

class UserManager : public QObject
{
    Q_OBJECT

public:
    explicit UserManager(QObject *parent = nullptr);
    ~UserManager();

    // 用户登录验证
    LoginResult userLogin(const QString& username, const QString& password);

    // 用户注册
    RegisterResult userRegister(const QString& username, const QString& password,
                               const QString& identity, const QString& gender,
                               int age, const QString& phone);

    // 获取当前登录用户信息
    UserInfo getCurrentUser() const { return m_currentUser; }

    // 用户登出
    void userLogout();

    // 验证输入信息格式
    bool validateUserInput(const QString& username, const QString& password,
                          const QString& gender, int age, const QString& phone);

signals:
    // 登录成功信号
    void loginSuccess(const UserInfo& userInfo);

    // 登录失败信号
    void loginFailed(LoginResult result);

    // 注册成功信号
    void registerSuccess();

    // 注册失败信号
    void registerFailed(RegisterResult result);

private slots:
    void onLoginTimeout();

private:
    DatabaseManager* m_databaseManager;
    UserInfo m_currentUser;
    QTimer* m_loginTimer;

    // 验证电话号码格式
    bool validatePhoneNumber(const QString& phone);

    // 验证用户名格式
    bool validateUsername(const QString& username);

    // 字符串转用户类型
    UserType stringToUserType(const QString& identity);
};

#endif // USERMANAGER_H
