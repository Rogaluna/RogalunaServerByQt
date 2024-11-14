#include "HeartbeatTimer.h"

#include <QSqlQuery>
#include <QDebug>
#include <iostream>

HeartbeatTimer::HeartbeatTimer(QSqlDatabase db, int intervalMs, QObject *parent)
    : QObject(parent), db(db)
{
    // 设置定时器的触发间隔
    timer.setInterval(intervalMs);
    timer.setSingleShot(false);

    // 连接定时器的超时信号到发送心跳包的槽函数
    connect(&timer, &QTimer::timeout, this, &HeartbeatTimer::sendHeartbeat);

}

void HeartbeatTimer::start()
{
    if (db.isOpen()) {
        qDebug() << "Starting heartbeat...";
        if (onStartCallback) {
            onStartCallback();
        }
        timer.start();
    } else {
        qWarning() << "Database is not open. Cannot start heartbeat.";
    }
}

void HeartbeatTimer::stop()
{
    qDebug() << "Stopping heartbeat...";
    timer.stop();
    if (onStopCallback) {
        onStopCallback();
    }
}

void HeartbeatTimer::setOnStartCallback(const std::function<void ()> &callback)
{
    onStartCallback = callback;
}

void HeartbeatTimer::setOnStopCallback(const std::function<void ()> &callback)
{
    onStopCallback = callback;
}

void HeartbeatTimer::sendHeartbeat()
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open. Stopping heartbeat...";
        stop();
        return;
    }

    QSqlQuery query(db);
    if (query.exec("SELECT 1")) {
        std::cout << "\rHeartbeat successful: Beng.Beng.Beng..." << std::flush;
    } else {
        std::cout << "\rHeartbeat failed:" << query.lastQuery().toStdString() << std::flush;
        stop();
    }
}
