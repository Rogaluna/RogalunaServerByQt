#ifndef ROGALUNADATABASESERVER_H
#define ROGALUNADATABASESERVER_H

#include "RogalunaDatabaseServer_Global.h"
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QMutex>
#include <QtSql/QSqlError>

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

    template <typename Func>
    bool executeTransaction(Func dbOperation)
    {
        // 开始事务
        if (!db.transaction()) {
            qWarning() << "Failed to start transaction:" << db.lastError().text();
            return false;
        }

        // 执行传入的数据库操作
        bool success = dbOperation();
        if (!success) {
            qWarning() << "Atomic operation failed, rolling back.";
            db.rollback();
            return false;
        }

        // 提交事务
        if (!db.commit()) {
            qWarning() << "Failed to commit transaction:" << db.lastError().text();
            db.rollback();
            return false;
        }

        return true;
    }

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

private:

};

#endif // ROGALUNADATABASESERVER_H
