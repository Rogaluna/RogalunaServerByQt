#ifndef ROGALUNADATABASESERVER_H
#define ROGALUNADATABASESERVER_H

#include "RogalunaDatabaseServer_Global.h"
#include <QtSql/QSqlDatabase>

class HeartbeatTimer;

class ROGALUNADATABASESERVER_EXPORT RogalunaDatabaseServer
{
public:
    RogalunaDatabaseServer(
        const QString& hostname,
        const QString& dbname,
        const QString& username,
        const QString& password,
        int port = 5432,
        int intervalMs = 5000,
        int maxReconnectAttempts = 5,
        int reconnectIntervalMs = 3000);
    ~RogalunaDatabaseServer();

    bool connect();
    void disconnect();
    bool isConnected();

    QSqlDatabase& getDatabase();

private:
    void onHeartbeatStop();

private:
    QSqlDatabase db;
    QString hostname;
    QString dbname;
    QString username;
    QString password;
    int port;

private:
    HeartbeatTimer* heartbeatTimer = nullptr;


    int reconnectAttempts = 0; // 重连计数器
    const int maxReconnectAttempts;
    const int reconnectIntervalMs; // 每次重连间隔时间
};

#endif // ROGALUNADATABASESERVER_H
