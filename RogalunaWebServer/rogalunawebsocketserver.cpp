#include "rogalunawebsocketserver.h"

#include <RogalunaStorageServer.h>
#include <QWebSocket>

RogalunaWebSocketServer::RogalunaWebSocketServer(const QString &webRootPath, RogalunaStorageServer *storageServer, QObject *parent)
    : QWebSocketServer{QStringLiteral("WebSocket Server"), QWebSocketServer::NonSecureMode, parent}
{
    Q_UNUSED(webRootPath)
    Q_UNUSED(storageServer)

}

void RogalunaWebSocketServer::start(quint16 port)
{
    // WebSocket 监听
    while(!listen(QHostAddress::Any, port)) {
        qDebug() << QString("port %1 has been occupied").arg(port);
        port += 1;
    }

    connect(this, &QWebSocketServer::newConnection, this, &RogalunaWebSocketServer::onNewWebSocketConnection);

    qDebug() << QString("Server will listen on port %1").arg(port);
}

void RogalunaWebSocketServer::onNewWebSocketConnection()
{
    QWebSocket *client = nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived, this, &RogalunaWebSocketServer::onWebSocketTextMessageReceived);
    connect(client, &QWebSocket::binaryMessageReceived, this, &RogalunaWebSocketServer::onWebSocketBinaryMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &RogalunaWebSocketServer::onWebSocketDisconnected);

    webSocketClients << client;

    qDebug() << "New WebSocket client connected";
}

void RogalunaWebSocketServer::onWebSocketTextMessageReceived(const QString &message)
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        qDebug() << "Received text message:" << message;
        client->sendTextMessage("Message received");
    }
}

void RogalunaWebSocketServer::onWebSocketBinaryMessageReceived(const QByteArray &message)
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        qDebug() << "Received binary message of size:" << message.size();
        // 在此处处理收到的二进制消息
    }
}

void RogalunaWebSocketServer::onWebSocketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        webSocketClients.removeAll(client);
        client->deleteLater();
        qDebug() << "WebSocket client disconnected";
    }
}
