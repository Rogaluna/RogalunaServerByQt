#include "RogalunaDatabaseServer.h"
#include <QtSql/QSqlQuery>
#include <HeartbeatTimer.h>
#include <QThread>

RogalunaDatabaseServer::RogalunaDatabaseServer(
   const QString& hostname,
   const QString& dbname,
   const QString& username,
   const QString& password,
   int port,
   int intervalMs,
   int maxReconnectAttempts,
   int reconnectIntervalMs)
    : hostname(hostname)
    , dbname(dbname)
    , username(username)
    , password(password)
    , port(port)
    , maxReconnectAttempts(maxReconnectAttempts)
    , reconnectIntervalMs(reconnectIntervalMs)
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

    heartbeatTimer = new HeartbeatTimer(db, intervalMs);
    heartbeatTimer->setOnStopCallback(std::bind(&RogalunaDatabaseServer::onHeartbeatStop, this));
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
    reconnectAttempts = 0; // 重置重连计数
    heartbeatTimer->start();
    return true;
}

void RogalunaDatabaseServer::disconnect()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool RogalunaDatabaseServer::isConnected()
{
    return db.isOpen() && db.isValid();
}

QSqlDatabase& RogalunaDatabaseServer::getDatabase()
{
    return db;
}

void RogalunaDatabaseServer::onHeartbeatStop()
{
    qDebug() << "Heartbeat stopped, attempting to reconnect...";
    heartbeatTimer->stop();

    const auto tryReconnect = [&]() {
        disconnect();
        return connect();
    };

    while (reconnectAttempts < maxReconnectAttempts) {
        qDebug() << "Reconnection attempt" << (reconnectAttempts + 1);
        if (tryReconnect()) {
            qDebug() << "Reconnected successfully.";
            heartbeatTimer->start();
            return;
        }
        reconnectAttempts++;
        QThread::sleep(reconnectIntervalMs / 1000); // 等待一段时间后重试
    }

    qWarning() << "Failed to reconnect after" << maxReconnectAttempts << "attempts.";
}
