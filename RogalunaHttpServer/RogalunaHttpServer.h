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
        const QString &_secretKey,
        quint16 _httpPort,
        bool _enableSsl,
        quint16 _httpsPort,
        const QString &_certFilePath,
        const QString &_keyFilePath,
        const QString &_sslEncryptAlg);

    bool start();

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

    quint16 httpPort;
    bool enableSsl;
    quint16 httpsPort;
    QString certFilePath;
    QString keyFilePath;
    QString sslEncryptAlg;
};

#endif // ROGALUNAHTTPSERVER_H
