#ifndef ROGALUNAWEBSOCKETSERVER_H
#define ROGALUNAWEBSOCKETSERVER_H

#include "RogalunaWebServer_global.h"
#include <QWebSocketServer>

class QWebSocket;
class QWebSocketServer;
class RogalunaStorageServer;

class ROGALUNAWEBSERVER_EXPORT RogalunaWebSocketServer : public QWebSocketServer
{
    Q_OBJECT
public:
    explicit RogalunaWebSocketServer(const QString &webRootPath, RogalunaStorageServer *storageServer, QObject *parent = nullptr);
    void start(quint16 port);

private slots:
    void onNewWebSocketConnection();
    void onWebSocketTextMessageReceived(const QString &message);
    void onWebSocketBinaryMessageReceived(const QByteArray &message);
    void onWebSocketDisconnected();

signals:

private:
    QWebSocketServer *webSocketServer;
    QList<QWebSocket *> webSocketClients;
};

#endif // ROGALUNAWEBSOCKETSERVER_H
