#include "RogalunaHttpServer.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QDebug>
#include <RogalunaHttpConfig.h>

#include <RequestHandlers/DefaultOptionsHandler.h>

#include <RequestHandlers/GetWebFileHandler.h>
#include <RequestHandlers/TestHandler.h>

#include <RequestHandlers/CloudDrive/GetUidFromPathHandler.h>
#include <RequestHandlers/CloudDrive/CreateFolderHandler.h>
#include <RequestHandlers/CloudDrive/FetchFileDirectLinkHandler.h>
#include <RequestHandlers/CloudDrive/GetFileHandler.h>
#include <RequestHandlers/CloudDrive/GetFileListHandler.h>
#include <RequestHandlers/CloudDrive/GetParentFolderHandler.h>
#include <RequestHandlers/CloudDrive/MergeFileHandler.h>
#include <RequestHandlers/CloudDrive/PostFileHandler.h>

#include <RequestHandlers/Account/LoginHandler.h>
#include <RequestHandlers/Account/RegisterHandler.h>

#include <RequestHandlers/Library/GetBookCategoriesHandler.h>

#include <RequestHandlers/MusicStation/GetMusicListHandler.h>

RogalunaHttpServer::RogalunaHttpServer(
    const QString &_webRootPath,
    const QString &_algorithm,
    const QString &_secretKey)

    : server(new QHttpServer())
    , webRootPath(_webRootPath)
    , algorithm(_algorithm)
    , secretKey(_secretKey)
{
    QString localWebRootPath = webRootPath;

    RogalunaHttpConfig::getInstance().setWebRootPath(&webRootPath);
    RogalunaHttpConfig::getInstance().setAlgorithm(&algorithm);
    RogalunaHttpConfig::getInstance().setSecretKey(&secretKey);
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

    qDebug() << "||||||||||||||||||||      CloudDrive     ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/cloudDrive/getFileList", QHttpServerRequest::Method::Get, GetFileListHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/postFile", QHttpServerRequest::Method::Post, PostFileHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/mergeFile", QHttpServerRequest::Method::Post, MergeFileHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/fetchFileDirectLink", QHttpServerRequest::Method::Post, FetchFileDirectLinkHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/getFile", QHttpServerRequest::Method::Get, GetFileHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/createFolder", QHttpServerRequest::Method::Post, CreateFolderHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/getParentFolder", QHttpServerRequest::Method::Get, GetParentFolderHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/getUidFromPath", QHttpServerRequest::Method::Get, GetUidFromPathHandler);

    qDebug() << "||||||||||||||||||||       Account       ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/account/register", QHttpServerRequest::Method::Post, RegisterHandler);
    REGISTER_ROUTE(server, "/api/account/login", QHttpServerRequest::Method::Post, LoginHandler);

    qDebug() << "||||||||||||||||||||       Library       ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/library/getBookCategories", QHttpServerRequest::Method::Get, GetBookCategoriesHandler);

    qDebug() << "||||||||||||||||||||     MusicStation    ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/musicStation/getMusicList", QHttpServerRequest::Method::Get, GetMusicListHandler);

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

void RogalunaHttpServer::setStorageServer(RogalunaStorageServer *_storageServer)
{
    RogalunaHttpConfig::getInstance().setStorageServer(_storageServer);
}

void RogalunaHttpServer::setDatabaseServer(RogalunaDatabaseServer *_databaseServer)
{
    RogalunaHttpConfig::getInstance().setDatabaseServer(_databaseServer);
}

void RogalunaHttpServer::setCloudDriveServer(RogalunaCloudDriveServer *_cloudDriveServer)
{
    RogalunaHttpConfig::getInstance().setCloudDriveServer(_cloudDriveServer);
}

void RogalunaHttpServer::setLibraryServer(RogalunaLibraryServer *_libraryServer)
{
    RogalunaHttpConfig::getInstance().setLibraryServer(_libraryServer);
}

void RogalunaHttpServer::setMusicServer(RogalunaMusicServer *_musicServer)
{
    RogalunaHttpConfig::getInstance().setMusicServer(_musicServer);
}
