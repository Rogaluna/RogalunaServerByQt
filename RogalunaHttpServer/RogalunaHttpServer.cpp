#include "RogalunaHttpServer.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QDebug>
#include <RogalunaHttpConfig.h>

#include <RequestHandlers/DefaultOptionsHandler.h>

#include <RequestHandlers/GetWebFileHandler.h>
#include <RequestHandlers/TestHandler.h>

#include <RequestHandlers/FileStorage/CreateFolderHandler.h>
#include <RequestHandlers/FileStorage/FetchFileDirectLinkHandler.h>
#include <RequestHandlers/FileStorage/GetFileHandler.h>
#include <RequestHandlers/FileStorage/GetFileListHandler.h>
#include <RequestHandlers/FileStorage/MergeFileHandler.h>
#include <RequestHandlers/FileStorage/PostFileHandler.h>

#include <RequestHandlers/Account/LoginHandler.h>
#include <RequestHandlers/Account/RegisterHandler.h>

#include <RequestHandlers/Library/GetBookCategories.h>

RogalunaHttpServer::RogalunaHttpServer(
    const QString &_webRootPath,
    const QString &_algorithm,
    const QString &_secretKey,
    RogalunaStorageServer *_storageServer,
    RogalunaDatabaseServer* _databaseServer,
    RogalunaCloudDriveServer* _cloudDriveServer,
    RogalunaLibraryServer* _libraryServer)

    : server(new QHttpServer())
    , webRootPath(_webRootPath)
    , algorithm(_algorithm)
    , secretKey(_secretKey)
    , storageServer(_storageServer)
    , databaseServer(_databaseServer)
{
    QString localWebRootPath = webRootPath;

    RogalunaHttpConfig::getInstance().setWebRootPath(&webRootPath);
    RogalunaHttpConfig::getInstance().setAlgorithm(&algorithm);
    RogalunaHttpConfig::getInstance().setSecretKey(&secretKey);
    RogalunaHttpConfig::getInstance().setStorageServer(storageServer);
    RogalunaHttpConfig::getInstance().setDatabaseServer(databaseServer);

    RogalunaHttpConfig::getInstance().setCloudDriveServer(_cloudDriveServer);
    RogalunaHttpConfig::getInstance().setLibraryServer(_libraryServer);
}

bool RogalunaHttpServer::start(quint16 port)
{
    // server->route("/", []() {
    //     return "Hello, welcome to your Qt HTTP server!";
    // });

    qDebug() << "|||||||||||||||||||| ***ROUTERS START*** ||||||||||||||||||||";

    qDebug() << "||||||||||||||||||||         Def         ||||||||||||||||||||";

    REGISTER_ROUTE(server, "", QHttpServerRequest::Method::Options, DefaultOptionsHandler);

    qDebug() << "||||||||||||||||||||         Web         ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/web/<arg>", QHttpServerRequest::Method::Get, GetWebFileHandler);

    qDebug() << "||||||||||||||||||||     FileStorage     ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/fileStorage/getFileList", QHttpServerRequest::Method::Get, GetFileListHandler);
    REGISTER_ROUTE(server, "/api/fileStorage/postFile", QHttpServerRequest::Method::Post, PostFileHandler);
    REGISTER_ROUTE(server, "/api/fileStorage/mergeFile", QHttpServerRequest::Method::Post, MergeFileHandler);
    REGISTER_ROUTE(server, "/api/fileStorage/fetchFileDirectLink", QHttpServerRequest::Method::Post, FetchFileDirectLinkHandler);
    REGISTER_ROUTE(server, "/api/fileStorage/getFile", QHttpServerRequest::Method::Get, GetFileHandler);
    REGISTER_ROUTE(server, "/api/fileStorage/createFolder", QHttpServerRequest::Method::Post, CreateFolderHandler);

    qDebug() << "||||||||||||||||||||       Account       ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/account/register", QHttpServerRequest::Method::Post, RegisterHandler);
    REGISTER_ROUTE(server, "/api/account/login", QHttpServerRequest::Method::Post, LoginHandler);

    qDebug() << "||||||||||||||||||||       Library       ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/library/getBookCategories", QHttpServerRequest::Method::Get, GetBookCategories);

    qDebug() << "||||||||||||||||||||  ***ROUTERS END***  ||||||||||||||||||||";

    // REGISTER_ROUTE(server, "/*", GetWebFileHandler, webRootPath);

    if (server->listen(QHostAddress::Any, port))
    {
        qDebug() << "Server running on port" << port;
        return true;
    }
    else
    {
        qDebug() << "Failed to listen on port" << port;
        return true;
    }
}
