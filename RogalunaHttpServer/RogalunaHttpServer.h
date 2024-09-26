#ifndef ROGALUNAHTTPSERVER_H
#define ROGALUNAHTTPSERVER_H

#include "RogalunaHttpServer_Global.h"

#include <QHttpServer>

class RogalunaLibraryServer;
class RogalunaCloudDriveServer;
class RogalunaStorageServer;
class RogalunaDatabaseServer;

// 注册路由
#define REGISTER_ROUTE(server, routePath, method, handlerClass) \
    do { \
        server->route(routePath, method, handlerClass::handleRequest); \
        qDebug() << "***Route*** " << routePath << "|" << #method;     \
    } while (0)

class ROGALUNAHTTPSERVER_EXPORT RogalunaHttpServer
{
public:
    RogalunaHttpServer(
        const QString &_webRootPath,
        const QString &_algorithm,
        const QString &_secretKey,
        RogalunaStorageServer *_storageServer,
        RogalunaDatabaseServer* _databaseServer,
        RogalunaCloudDriveServer* _cloudDriveServer,
        RogalunaLibraryServer* _libraryServer);

    bool start(quint16 port);

private:
    QHttpServer *server;

    QString webRootPath;
    QString algorithm;
    QString secretKey;
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
};

#endif // ROGALUNAHTTPSERVER_H
