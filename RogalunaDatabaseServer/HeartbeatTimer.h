#ifndef HEARTBEATTIMER_H
#define HEARTBEATTIMER_H

#include <QObject>
#include <QTimer>
#include <QSqlDatabase>

class HeartbeatTimer : public QObject
{
    Q_OBJECT
public:
    explicit HeartbeatTimer(QSqlDatabase db, int intervalMs, QObject *parent = nullptr);

    // 开始心跳
    void start();
    // 停止心跳
    void stop();

public:
    // 设置回调函数
    void setOnStartCallback(const std::function<void()>& callback);
    void setOnStopCallback(const std::function<void()>& callback);

private slots:
    void sendHeartbeat();

private:
    QSqlDatabase db;
    QTimer timer;

    std::function<void()> onStartCallback;
    std::function<void()> onStopCallback;
};

#endif // HEARTBEATTIMER_H
