#include "RogalunaHttpServer.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QDebug>
#include <RogalunaHttpConfig.h>
#include <RogalunaStorageServer.h>
#include <QSslKey>

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

#include <RequestHandlers/Account/GetUserInfoHandler.h>
#include <RequestHandlers/Account/LoginHandler.h>
#include <RequestHandlers/Account/RegisterHandler.h>

#include <RequestHandlers/Library/GetCategoriesHandler.h>
#include <RequestHandlers/Library/GetBookInfoHandler.h>
#include <RequestHandlers/Library/GetBookListHandler.h>
#include <RequestHandlers/Library/GetBookReadProgressHandler.h>
#include <RequestHandlers/Library/GetChapterContentHandler.h>
#include <RequestHandlers/Library/GetChapterListHandler.h>
#include <RequestHandlers/Library/NewBookHandler.h>
#include <RequestHandlers/Library/NewChapterHandler.h>
#include <RequestHandlers/Library/UpdateChapterContentHandler.h>
#include <RequestHandlers/Library/GetBookCategoriesHandler.h>
#include <RequestHandlers/Library/GetChapterInfoHandler.h>
#include <RequestHandlers/Library/UpdateBookInfoHandler.h>
#include <RequestHandlers/Library/UpdateChapterInfoHandler.h>
#include <RequestHandlers/Library/DeleteChapterHandler.h>
#include <RequestHandlers/Library/DeleteBookHandler.h>
#include <RequestHandlers/Library/GetLibraryResourceHandler.h>
#include <RequestHandlers/Library/UploadLibraryResourceHandler.h>

#include <RequestHandlers/MusicStation/GetMusicHandler.h>
#include <RequestHandlers/MusicStation/GetMusicListHandler.h>
#include <RequestHandlers/MusicStation/MergeMusicHandler.h>
#include <RequestHandlers/MusicStation/PostMusicHandler.h>

RogalunaHttpServer::RogalunaHttpServer(
    const QString &_webRootPath,
    const QString &_algorithm,
    const QString &_secretKey)

    : server(new QHttpServer())
    , webRootPath(_webRootPath)
    , algorithm(_algorithm)
    , secretKey(_secretKey)
{
    RogalunaHttpConfig::getInstance().setWebRootPath(&webRootPath);
    RogalunaHttpConfig::getInstance().setAlgorithm(&algorithm);
    RogalunaHttpConfig::getInstance().setSecretKey(&secretKey);
}

bool RogalunaHttpServer::start(quint16 httpPort,
                               quint16 httpsPort,
                               const QString &certFilePath,
                               const QString &keyFilePath)
{
    // server->route("/", []() {
    //     return "Hello, welcome to your Qt HTTP server!";
    // });

    qDebug() << "|||||||||||||||||||| ***ROUTERS START*** ||||||||||||||||||||";

    qDebug() << "||||||||||||||||||||         Def         ||||||||||||||||||||";

    REGISTER_ROUTE(server, "", QHttpServerRequest::Method::Options, DefaultOptionsHandler);

    qDebug() << "||||||||||||||||||||         Web         ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/", QHttpServerRequest::Method::Get, RedirectToWebHandler);
    REGISTER_ROUTE(server, "/web", QHttpServerRequest::Method::Get, RedirectToWebHandler);
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
    REGISTER_ROUTE(server, "/api/account/getUserInfo", QHttpServerRequest::Method::Get, GetUserInfoHandler);

    qDebug() << "||||||||||||||||||||       Library       ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/library/getCategories", QHttpServerRequest::Method::Get, GetCategoriesHandler);
    REGISTER_ROUTE(server, "/api/library/getBookList", QHttpServerRequest::Method::Get, GetBookListHandler);
    REGISTER_ROUTE(server, "/api/library/getBookInfo", QHttpServerRequest::Method::Get, GetBookInfoHandler);
    REGISTER_ROUTE(server, "/api/library/getBookCategories", QHttpServerRequest::Method::Get, GetBookCategoriesHandler);
    REGISTER_ROUTE(server, "/api/library/getChapterList", QHttpServerRequest::Method::Get, GetChapterListHandler);
    REGISTER_ROUTE(server, "/api/library/getChapterContent", QHttpServerRequest::Method::Get, GetChapterContentHandler);
    REGISTER_ROUTE(server, "/api/library/getChapterInfo", QHttpServerRequest::Method::Get, GetChapterInfoHandler);
    REGISTER_ROUTE(server, "/api/library/getBookReadProgress", QHttpServerRequest::Method::Get, GetBookReadProgressHandler);

    REGISTER_ROUTE(server, "/api/library/newBook", QHttpServerRequest::Method::Post, NewBookHandler);
    REGISTER_ROUTE(server, "/api/library/newChapter", QHttpServerRequest::Method::Post, NewChapterHandler);
    REGISTER_ROUTE(server, "/api/library/updateBookInfo", QHttpServerRequest::Method::Post, UpdateBookInfoHandler);
    REGISTER_ROUTE(server, "/api/library/updateChapterContent", QHttpServerRequest::Method::Post, UpdateChapterContentHandler);
    REGISTER_ROUTE(server, "/api/library/updateChapterInfo", QHttpServerRequest::Method::Post, UpdateChapterInfoHandler);

    REGISTER_ROUTE(server, "/api/library/deleteChapter", QHttpServerRequest::Method::Delete, DeleteChapterHandler);
    REGISTER_ROUTE(server, "/api/library/deleteBook", QHttpServerRequest::Method::Delete, DeleteBookHandler);

    REGISTER_ROUTE(server, "/api/library/uploadResource", QHttpServerRequest::Method::Post, UploadLibraryResourceHandler);
    REGISTER_ROUTE(server, "/api/library/getResource", QHttpServerRequest::Method::Get, GetLibraryResourceHandler);

    qDebug() << "||||||||||||||||||||     MusicStation    ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/musicStation/getMusic", QHttpServerRequest::Method::Get, GetMusicHandler);
    REGISTER_ROUTE(server, "/api/musicStation/getMusicList", QHttpServerRequest::Method::Get, GetMusicListHandler);
    REGISTER_ROUTE(server, "/api/musicStation/postMusic", QHttpServerRequest::Method::Post, PostMusicHandler);
    REGISTER_ROUTE(server, "/api/musicStation/mergeMusic", QHttpServerRequest::Method::Post, MergeMusicHandler);

    qDebug() << "||||||||||||||||||||  ***ROUTERS END***  ||||||||||||||||||||";

    // REGISTER_ROUTE(server, "/*", GetWebFileHandler, webRootPath);

    // 监听 http
    if (httpPort >= 0x0 && httpPort <= 0xFFFF) {
        if (!server->listen(QHostAddress::Any, httpPort))
        {
            qDebug() << "Failed to listen on port:" << httpPort;
            return false;
        }
        qDebug() << "Http server running on port:" << httpPort;
    } else {
        qDebug() << "Illegal port, http server closed";
    }

    // 监听 https
    if (httpsPort >= 0x0 && httpsPort <= 0xFFFF) {
        // 获取证书和私钥
        QFile certFile(certFilePath);
        QFile keyFile(keyFilePath);
        if (!certFile.open(QIODevice::ReadOnly) || !keyFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Unable to open certificate or private key file!";
            return false;
        }

        QSslCertificate certificate(&certFile);
        QSslKey privateKey(&keyFile, QSsl::Rsa);

        QSslConfiguration sslConfiguration;
        sslConfiguration.setLocalCertificate(certificate);
        sslConfiguration.setPrivateKey(privateKey);
        sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);

        server->sslSetup(sslConfiguration);

        if (!server->listen(QHostAddress::Any, httpsPort))
        {
            qDebug() << "Failed to listen on port!" << httpsPort;
            return false;
        }
        qDebug() << "Https server running on port." << httpsPort;
    } else {
        qDebug() << "Illegal port, https server closed";
    }

    return true;
}

void RogalunaHttpServer::setStorageServer(RogalunaStorageServer *_storageServer)
{
    RogalunaHttpConfig::getInstance().setStorageServer(_storageServer);
}

void RogalunaHttpServer::setDatabaseServer(RogalunaDatabaseServer *_databaseServer)
{
    RogalunaHttpConfig::getInstance().setDatabaseServer(_databaseServer);
}

void RogalunaHttpServer::setAccountServer(RogalunaAccountServer *_accountServer)
{
    RogalunaHttpConfig::getInstance().setAccountServer(_accountServer);
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

void RogalunaHttpServer::postInitialization()
{
    // 后初始化，在设置了配置变量之后调用

    // 检查 webRootPath 所标识的文件夹是否存在，不存在则创建一个
    QString rootPath = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(webRootPath);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(".");  // 创建根文件夹
    }
}
