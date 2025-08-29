#include "DatabaseManager.h"
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QDir>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_connectionName("MedicalSystemDB")
{
    initDatabase();
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::initDatabase()
{
    // 使用SQLite数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);

    // 设置数据库文件路径
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dbPath)) {
        dir.mkpath(dbPath);
    }

    m_database.setDatabaseName(dbPath + "/medical_system.db");

    if (!m_database.open()) {
        qDebug() << "数据库连接失败:" << m_database.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功";

    // 创建用户表
    return createUserTable();
}

bool DatabaseManager::createUserTable()
{
    QSqlQuery query(m_database);
    QString createTableSQL =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "identity INTEGER NOT NULL, "
        "gender TEXT NOT NULL, "
        "age INTEGER NOT NULL, "
        "phone TEXT NOT NULL, "
        "created_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createTableSQL)) {
        qDebug() << "创建用户表失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "用户表创建成功";
    return true;
}

bool DatabaseManager::insertUser(const UserInfo& userInfo)
{
    // 检查用户是否已存在
    if (userExists(userInfo.username)) {
        qDebug() << "用户已存在:" << userInfo.username;
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare("INSERT INTO users (username, password, identity, gender, age, phone) "
                  "VALUES (:username, :password, :identity, :gender, :age, :phone)");

    query.bindValue(":username", userInfo.username);
    query.bindValue(":password", encryptPassword(userInfo.password));
    query.bindValue(":identity", static_cast<int>(userInfo.identity));
    query.bindValue(":gender", userInfo.gender);
    query.bindValue(":age", userInfo.age);
    query.bindValue(":phone", userInfo.phone);

    if (!query.exec()) {
        qDebug() << "插入用户失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "用户注册成功:" << userInfo.username;
    return true;
}

LoginResult DatabaseManager::validateUser(const QString& username, const QString& password, UserInfo& userInfo)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT username, password, identity, gender, age, phone FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "查询用户失败:" << query.lastError().text();
        return LOGIN_DATABASE_ERROR;
    }

    if (!query.next()) {
        qDebug() << "用户不存在:" << username;
        return LOGIN_USER_NOT_EXIST;
    }

    // 验证密码
    QString storedPassword = query.value("password").toString();
    if (storedPassword != encryptPassword(password)) {
        qDebug() << "密码错误:" << username;
        return LOGIN_PASSWORD_ERROR;
    }

    // 填充用户信息
    userInfo.username = query.value("username").toString();
    userInfo.password = password; // 不返回加密密码
    userInfo.identity = static_cast<UserType>(query.value("identity").toInt());
    userInfo.gender = query.value("gender").toString();
    userInfo.age = query.value("age").toInt();
    userInfo.phone = query.value("phone").toString();

    qDebug() << "用户登录成功:" << username;
    return LOGIN_SUCCESS;
}

bool DatabaseManager::userExists(const QString& username)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

QString DatabaseManager::encryptPassword(const QString& password)
{
    // 使用SHA256加密密码
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.toUtf8());
    return hash.result().toHex();
}

void DatabaseManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
        qDebug() << "数据库连接已关闭";
    }
}
