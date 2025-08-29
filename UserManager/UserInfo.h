#ifndef USERINFO_H
#define USERINFO_H

#include <QString>

// 用户身份枚举
enum UserType {
    PATIENT = 0,    // 患者
    DOCTOR = 1      // 医生
};

// 用户信息结构体
struct UserInfo {
    QString username;    // 用户名
    QString password;    // 密码
    UserType identity;   // 身份(患者/医生)
    QString gender;      // 性别
    int age;            // 年龄
    QString phone;      // 电话

    UserInfo() : age(0), identity(PATIENT) {}

    UserInfo(const QString& user, const QString& pwd, UserType type,
             const QString& gen, int a, const QString& ph)
        : username(user), password(pwd), identity(type), gender(gen), age(a), phone(ph) {}
};

// 登录结果枚举
enum LoginResult {
    LOGIN_SUCCESS = 0,      // 登录成功
    LOGIN_USER_NOT_EXIST,   // 用户不存在
    LOGIN_PASSWORD_ERROR,   // 密码错误
    LOGIN_DATABASE_ERROR    // 数据库错误
};

// 注册结果枚举
enum RegisterResult {
    REGISTER_SUCCESS = 0,       // 注册成功
    REGISTER_USER_EXISTS,       // 用户已存在
    REGISTER_DATABASE_ERROR,    // 数据库错误
    REGISTER_INVALID_INFO      // 信息不完整
};

#endif // USERINFO_H
