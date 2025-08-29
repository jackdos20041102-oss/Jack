#include "UserManager.h"
#include <QRegularExpression>
#include <QDebug>

UserManager::UserManager(QObject *parent)
    : QObject(parent)
    , m_databaseManager(new DatabaseManager(this))
    , m_loginTimer(new QTimer(this))
{
    // 设置登录超时时间（30分钟）
    m_loginTimer->setSingleShot(true);
    m_loginTimer->setInterval(30 * 60 * 1000);
    connect(m_loginTimer, &QTimer::timeout, this, &UserManager::onLoginTimeout);
}

UserManager::~UserManager()
{
    delete m_databaseManager;
}

LoginResult UserManager::userLogin(const QString& username, const QString& password)
{
    // 验证输入是否为空
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "用户名或密码为空";
        emit loginFailed(LOGIN_USER_NOT_EXIST);
        return LOGIN_USER_NOT_EXIST;
    }

    // 验证用户信息
    UserInfo userInfo;
    LoginResult result = m_databaseManager->validateUser(username, password, userInfo);

    if (result == LOGIN_SUCCESS) {
        m_currentUser = userInfo;
        m_loginTimer->start(); // 开始登录超时计时
        emit loginSuccess(userInfo);
        qDebug() << "用户登录成功:" << username << "身份:" << (userInfo.identity == PATIENT ? "患者" : "医生");
    } else {
        emit loginFailed(result);
        qDebug() << "用户登录失败:" << username << "错误代码:" << result;
    }

    return result;
}

RegisterResult UserManager::userRegister(const QString& username, const QString& password,
                                        const QString& identity, const QString& gender,
                                        int age, const QString& phone)
{
    // 验证输入信息格式
    if (!validateUserInput(username, password, gender, age, phone)) {
        emit registerFailed(REGISTER_INVALID_INFO);
        return REGISTER_INVALID_INFO;
    }

    // 检查用户是否已存在
    if (m_databaseManager->userExists(username)) {
        emit registerFailed(REGISTER_USER_EXISTS);
        return REGISTER_USER_EXISTS;
    }

    // 创建用户信息
    UserInfo userInfo(username, password, stringToUserType(identity), gender, age, phone);

    // 插入数据库
    if (m_databaseManager->insertUser(userInfo)) {
        emit registerSuccess();
        qDebug() << "用户注册成功:" << username;
        return REGISTER_SUCCESS;
    } else {
        emit registerFailed(REGISTER_DATABASE_ERROR);
        return REGISTER_DATABASE_ERROR;
    }
}

void UserManager::userLogout()
{
    m_loginTimer->stop();
    m_currentUser = UserInfo(); // 清空当前用户信息
    qDebug() << "用户已登出";
}

bool UserManager::validateUserInput(const QString& username, const QString& password,
                                   const QString& gender, int age, const QString& phone)
{
    // 验证用户名
    if (!validateUsername(username)) {
        qDebug() << "用户名格式错误";
        return false;
    }

    // 验证密码长度
    if (password.length() < 6 || password.length() > 20) {
        qDebug() << "密码长度应在6-20位之间";
        return false;
    }

    // 验证性别
    if (gender != "男" && gender != "女") {
        qDebug() << "性别信息错误";
        return false;
    }

    // 验证年龄
    if (age < 1 || age > 120) {
        qDebug() << "年龄信息错误";
        return false;
    }

    // 验证电话号码
    if (!validatePhoneNumber(phone)) {
        qDebug() << "电话号码格式错误";
        return false;
    }

    return true;
}

bool UserManager::validatePhoneNumber(const QString& phone)
{
    // 中国大陆手机号码正则表达式
    QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    return phoneRegex.match(phone).hasMatch();
}

bool UserManager::validateUsername(const QString& username)
{
    // 用户名：3-20位字母、数字、下划线
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }

    QRegularExpression usernameRegex("^[a-zA-Z0-9_]+$");
    return usernameRegex.match(username).hasMatch();
}

UserType UserManager::stringToUserType(const QString& identity)
{
    if (identity == "医生" || identity == "doctor") {
        return DOCTOR;
    }
    return PATIENT; // 默认为患者
}

void UserManager::onLoginTimeout()
{
    qDebug() << "登录超时，自动登出";
    userLogout();
}
