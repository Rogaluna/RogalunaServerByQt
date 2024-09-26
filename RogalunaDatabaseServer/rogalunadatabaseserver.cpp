#include "RogalunaDatabaseServer.h"
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

RogalunaDatabaseServer::RogalunaDatabaseServer(const QString& hostname, const QString& dbname, const QString& username, const QString& password, int port)
    : hostname(hostname), dbname(dbname), username(username), password(password), port(port)
{

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(hostname);
    db.setDatabaseName(dbname);
    db.setUserName(username);
    db.setPassword(password);
    db.setPort(port);

    // QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    // db.setHostName("117.72.65.176");  // 如果是远程数据库，请使用相应的IP地址
    // db.setDatabaseName("test");  // 数据库名称
    // db.setUserName("postgres");  // 数据库用户名
    // db.setPassword("Lxzx546495");  // 数据库密码
    // db.setPort(5432);  // 数据库端口，默认是5432
}

RogalunaDatabaseServer::~RogalunaDatabaseServer()
{
    disconnect();
}

bool RogalunaDatabaseServer::connect()
{
    if (!db.open()) {
        qDebug() << "Database connection error:" << db.lastError();
        return false;
    }
    return true;
}

void RogalunaDatabaseServer::disconnect()
{
    if (db.isOpen()) {
        db.close();
    }
}

QSqlDatabase& RogalunaDatabaseServer::getDatabase()
{
    return db;
}

