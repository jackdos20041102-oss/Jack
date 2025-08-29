#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include "UserInfo.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    // 初始化数据库连接
    bool initDatabase();

    // 创建用户表
    bool createUserTable();

    // 用户注册 - 插入用户信息
    bool insertUser(const UserInfo& userInfo);

    // 用户登录 - 验证用户信息
    LoginResult validateUser(const QString& username, const QString& password, UserInfo& userInfo);

    // 检查用户是否存在
    bool userExists(const QString& username);

    // 关闭数据库连接
    void closeDatabase();

private:
    QSqlDatabase m_database;
    QString m_connectionName;

    // 密码加密函数
    QString encryptPassword(const QString& password);
};

#endif // DATABASEMANAGER_H
