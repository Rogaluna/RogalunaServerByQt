#include "rogalunahttpserver.h"
#include <RogalunaStorageServer.h>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <RequestHandlers/getwebfilehandler.h>
#include <RequestHandlers/FileStorage/getfilehandler.h>
#include <RequestHandlers/FileStorage/getfilelisthandler.h>
#include <RequestHandlers/FileStorage/postfilehandler.h>

RogalunaHttpServer::RogalunaHttpServer(const QString &webRootPath, RogalunaStorageServer *storageServer, QObject *parent)
    : QTcpServer(parent)
    , router()
{
    // router.addRoute("POST", "/", new PostRequestHandler());
    // router.addRoute("GET", "/list", new GetFileListHandler());

    router.addRoute("GET", "*", new GetWebFileHandler(webRootPath));
    router.addRoute("GET", "/fileStorage/getFileList", new GetFileListHandler(storageServer));
    router.addRoute("GET", "/fileStorage/getFile", new GetFileHandler(storageServer));
    router.addRoute("POST", "/fileStorage/postFile", new PostFileHandler(storageServer));
}

void RogalunaHttpServer::start(quint16 port)
{
    // Http 服务监听
    while(!listen(QHostAddress::Any, port)) {
        qDebug() << QString("port %1 has been occupied").arg(port);
        port += 1;
    }

    qDebug() << QString("Server will listen on port %1").arg(port);
}

void RogalunaHttpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        connect(socket, &QTcpSocket::readyRead, this, &RogalunaHttpServer::handleClient);
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    } else {
        delete socket;
    }
}

void RogalunaHttpServer::handleClient()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        processRequest(socket);
    }
}

void RogalunaHttpServer::processRequest(QTcpSocket *socket)
{
    QByteArray requestData = socket->readAll();
    QString request(requestData);

    qDebug() << "Request: " << request;

    QStringList lines = request.split("\r\n");
    QStringList requestLine = lines[0].split(' ');
    if (requestLine.size() >= 2) {
        QString method = requestLine[0];
        QString path = requestLine[1];
        QUrlQuery query;

        // 获取处理策略
        WebRequestHandler *handler = router.getHandler(method, path, query);

        if (handler) {
            handler->handleRequest(socket, path, query, requestData);
        } else {
            QByteArray response = "HTTP/1.1 404 Not Found\r\n\r\n";
            socket->write(response);
            socket->flush();
            socket->disconnectFromHost();
        }
    }
}
