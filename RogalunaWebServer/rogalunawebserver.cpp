#include "rogalunawebserver.h"

#include <RogalunaHttpServer.h>
#include <RogalunaWebSocketServer.h>

RogalunaWebServer::RogalunaWebServer(const QString &webRootPath, RogalunaStorageServer *storageServer, QObject *parent)
    : QObject{parent}
    , webRootPath(webRootPath)
    , storageServer(storageServer)
    , router()
    , httpServer(nullptr)
    , webSocketServer(nullptr)
{
    httpServer = new RogalunaHttpServer(webRootPath, storageServer);
    webSocketServer = new RogalunaWebSocketServer(webRootPath, storageServer);
}

void RogalunaWebServer::start(quint16 port)
{
    httpServer->start(port);
    webSocketServer->start(port+1);
}
