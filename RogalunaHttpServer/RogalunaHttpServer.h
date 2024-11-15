#ifndef ROGALUNAHTTPSERVER_H
#define ROGALUNAHTTPSERVER_H

#include "RogalunaHttpServer_Global.h"

#include <QHttpServer>

class RogalunaMusicServer;
class RogalunaLibraryServer;
class RogalunaAccountServer;
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
        const QString &_secretKey);

    bool start(quint16 port);

public:
    void setStorageServer(RogalunaStorageServer *_storageServer);
    void setDatabaseServer(RogalunaDatabaseServer* _databaseServer);

    void setAccountServer(RogalunaAccountServer* _accountServer);
    void setCloudDriveServer(RogalunaCloudDriveServer* _cloudDriveServer);
    void setLibraryServer(RogalunaLibraryServer* _libraryServer);
    void setMusicServer(RogalunaMusicServer* _musicServer);

    void postInitialization();

private:
    QHttpServer *server;

    QString webRootPath;
    QString algorithm;
    QString secretKey;
};

#endif // ROGALUNAHTTPSERVER_H
