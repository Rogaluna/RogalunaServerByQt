#ifndef ROGALUNAWEBSERVER_H
#define ROGALUNAWEBSERVER_H

#include "RogalunaWebServer_global.h"
#include <QObject>
#include <Router.h>

class RogalunaWebSocketServer;
class RogalunaHttpServer;
class RogalunaStorageServer;

class ROGALUNAWEBSERVER_EXPORT RogalunaWebServer : public QObject
{
    Q_OBJECT

public:
    explicit RogalunaWebServer(const QString &webRootPath, RogalunaStorageServer *storageServer, QObject *parent = nullptr);

    void start(quint16 port);

signals:

private:
    QString webRootPath;
    RogalunaStorageServer *storageServer;

    Router router;

    RogalunaHttpServer* httpServer;
    RogalunaWebSocketServer* webSocketServer;
};

#endif // ROGALUNAWEBSERVER_H
