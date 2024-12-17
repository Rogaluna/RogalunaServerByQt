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
#include <RequestHandlers/CloudDrive/RenameFileHandler.h>
#include <RequestHandlers/CloudDrive/DeleteFileHandler.h>

#include <RequestHandlers/Account/GetAccountInfoHandler.h>
#include <RequestHandlers/Account/LoginHandler.h>
#include <RequestHandlers/Account/ModifyAccountInfoHandler.h>
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
#include <RequestHandlers/Library/GetChapterResourceHandler.h>
#include <RequestHandlers/Library/UploadChapterResourceHandler.h>
#include <RequestHandlers/Library/GetBookCoverHandler.h>
#include <RequestHandlers/Library/UploadBookCoverHandler.h>

#include <RequestHandlers/MusicStation/GetMusicHandler.h>
#include <RequestHandlers/MusicStation/GetMusicListHandler.h>
#include <RequestHandlers/MusicStation/MergeMusicHandler.h>
#include <RequestHandlers/MusicStation/PostMusicHandler.h>
#include <RequestHandlers/MusicStation/GetMusicCoverHandler.h>
#include <RequestHandlers/MusicStation/GetAlbumsInfoHandler.h>

RogalunaHttpServer::RogalunaHttpServer(
    const QString &_webRootPath,
    const QString &_algorithm,
    const QString &_secretKey,
    quint16 _httpPort,
    bool _enableSsl,
    quint16 _httpsPort,
    const QString &_certFilePath,
    const QString &_keyFilePath,
    const QString &_SslEncryptAlg)

    : server(new QHttpServer())
    , webRootPath(_webRootPath)
    , algorithm(_algorithm)
    , secretKey(_secretKey)
    , httpPort(_httpPort)
    , enableSsl(_enableSsl)
    , httpsPort(_httpsPort)
    , certFilePath(_certFilePath)
    , keyFilePath(_keyFilePath)
    , sslEncryptAlg(_SslEncryptAlg)
{
    RogalunaHttpConfig::getInstance().setWebRootPath(&webRootPath);
    RogalunaHttpConfig::getInstance().setAlgorithm(&algorithm);
    RogalunaHttpConfig::getInstance().setSecretKey(&secretKey);
}

bool RogalunaHttpServer::start()
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

    REGISTER_ROUTE(server, "/api/cloudDrive/getFileList", QHttpServerRequest::Method::Get, CloudDrive::GetFileListHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/postFile", QHttpServerRequest::Method::Post, CloudDrive::PostFileHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/mergeFile", QHttpServerRequest::Method::Post, CloudDrive::MergeFileHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/fetchFileDirectLink", QHttpServerRequest::Method::Post, CloudDrive::FetchFileDirectLinkHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/getFile", QHttpServerRequest::Method::Get, CloudDrive::GetFileHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/createFolder", QHttpServerRequest::Method::Post, CloudDrive::CreateFolderHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/getParentFolder", QHttpServerRequest::Method::Get, CloudDrive::GetParentFolderHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/getUidFromPath", QHttpServerRequest::Method::Get, CloudDrive::GetUidFromPathHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/renameFile", QHttpServerRequest::Method::Post, CloudDrive::RenameFileHandler);
    REGISTER_ROUTE(server, "/api/cloudDrive/deleteFile", QHttpServerRequest::Method::Delete, CloudDrive::DeleteFileHandler);

    qDebug() << "||||||||||||||||||||       Account       ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/account/register", QHttpServerRequest::Method::Post, Account::RegisterHandler);
    REGISTER_ROUTE(server, "/api/account/login", QHttpServerRequest::Method::Post, Account::LoginHandler);
    REGISTER_ROUTE(server, "/api/account/getAccountInfo", QHttpServerRequest::Method::Get, Account::GetAccountInfoHandler);
    REGISTER_ROUTE(server, "/api/account/modifyAccountInfo", QHttpServerRequest::Method::Post, Account::ModifyAccountInfoHandler);

    qDebug() << "||||||||||||||||||||       Library       ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/library/getCategories", QHttpServerRequest::Method::Get, Library::GetCategoriesHandler);
    REGISTER_ROUTE(server, "/api/library/getBookList", QHttpServerRequest::Method::Get, Library::GetBookListHandler);
    REGISTER_ROUTE(server, "/api/library/getBookInfo", QHttpServerRequest::Method::Get, Library::GetBookInfoHandler);
    REGISTER_ROUTE(server, "/api/library/getBookCategories", QHttpServerRequest::Method::Get, Library::GetBookCategoriesHandler);
    REGISTER_ROUTE(server, "/api/library/getChapterList", QHttpServerRequest::Method::Get, Library::GetChapterListHandler);
    REGISTER_ROUTE(server, "/api/library/getChapterContent", QHttpServerRequest::Method::Get, Library::GetChapterContentHandler);
    REGISTER_ROUTE(server, "/api/library/getChapterInfo", QHttpServerRequest::Method::Get, Library::GetChapterInfoHandler);
    REGISTER_ROUTE(server, "/api/library/getBookReadProgress", QHttpServerRequest::Method::Get, Library::GetBookReadProgressHandler);

    REGISTER_ROUTE(server, "/api/library/newBook", QHttpServerRequest::Method::Post, Library::NewBookHandler);
    REGISTER_ROUTE(server, "/api/library/newChapter", QHttpServerRequest::Method::Post, Library::NewChapterHandler);
    REGISTER_ROUTE(server, "/api/library/updateBookInfo", QHttpServerRequest::Method::Post, Library::UpdateBookInfoHandler);
    REGISTER_ROUTE(server, "/api/library/updateChapterContent", QHttpServerRequest::Method::Post, Library::UpdateChapterContentHandler);
    REGISTER_ROUTE(server, "/api/library/updateChapterInfo", QHttpServerRequest::Method::Post, Library::UpdateChapterInfoHandler);

    REGISTER_ROUTE(server, "/api/library/deleteChapter", QHttpServerRequest::Method::Delete, Library::DeleteChapterHandler);
    REGISTER_ROUTE(server, "/api/library/deleteBook", QHttpServerRequest::Method::Delete, Library::DeleteBookHandler);

    REGISTER_ROUTE(server, "/api/library/uploadResource", QHttpServerRequest::Method::Post, Library::UploadChapterResourceHandler);
    REGISTER_ROUTE(server, "/api/library/getResource", QHttpServerRequest::Method::Get, Library::GetChapterResourceHandler);
    REGISTER_ROUTE(server, "/api/library/uploadBookCover", QHttpServerRequest::Method::Post, Library::UploadBookCoverHandler);
    REGISTER_ROUTE(server, "/api/library/getBookCover", QHttpServerRequest::Method::Get, Library::GetBookCoverHandler);

    qDebug() << "||||||||||||||||||||     MusicStation    ||||||||||||||||||||";

    REGISTER_ROUTE(server, "/api/musicStation/getMusic", QHttpServerRequest::Method::Get, MusicStation::GetMusicHandler);
    REGISTER_ROUTE(server, "/api/musicStation/getMusicList", QHttpServerRequest::Method::Get, MusicStation::GetMusicListHandler);
    REGISTER_ROUTE(server, "/api/musicStation/postMusic", QHttpServerRequest::Method::Post, MusicStation::PostMusicHandler);
    REGISTER_ROUTE(server, "/api/musicStation/mergeMusic", QHttpServerRequest::Method::Post, MusicStation::MergeMusicHandler);
    REGISTER_ROUTE(server, "/api/musicStation/getCover", QHttpServerRequest::Method::Get, MusicStation::GetMusicCoverHandler);
    REGISTER_ROUTE(server, "/api/musicStation/getAlbumsInfo", QHttpServerRequest::Method::Get, MusicStation::GetAlbumsInfoHandler);

    qDebug() << "||||||||||||||||||||  ***ROUTERS END***  ||||||||||||||||||||";

    // REGISTER_ROUTE(server, "/*", GetWebFileHandler, webRootPath);

    // 监听 http
    if (!server->listen(QHostAddress::Any, httpPort))
    {
        qDebug() << "Failed to listen on port:" << httpPort;
        return false;
    }
    qDebug() << "Http server running on port:" << httpPort;

    if (enableSsl) {
        // 获取证书和私钥
        QFile certFile(certFilePath);
        QFile keyFile(keyFilePath);
        if (!certFile.open(QIODevice::ReadOnly) || !keyFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Unable to open certificate or private key file!";
            return false;
        }

        QSsl::KeyAlgorithm keyAlg = QSsl::Ec;

        if (sslEncryptAlg.compare("Rsa", Qt::CaseInsensitive) == 0) {
            keyAlg = QSsl::Rsa;
        } else if (sslEncryptAlg.compare("Ec", Qt::CaseInsensitive) == 0) {
            keyAlg = QSsl::Ec;
        } else if (sslEncryptAlg.compare("Dsa", Qt::CaseInsensitive) == 0) {
            keyAlg = QSsl::Dsa;
        } else if (sslEncryptAlg.compare("Dh", Qt::CaseInsensitive) == 0) {
            keyAlg = QSsl::Dh;
        } else if (sslEncryptAlg.compare("Opaque", Qt::CaseInsensitive) == 0) {
            keyAlg = QSsl::Opaque;
        } else {
            qWarning() << "Unsupported key encryption algorithm:" << sslEncryptAlg;
            qDebug() << "Key encryption algorithm will use \"Ec\"";
        }

        QSslCertificate certificate(&certFile);
        QSslKey privateKey(&keyFile, keyAlg);

        if (certificate.isNull()) {
            qDebug() << "Certificate is null. Check file path and format.";
            return false;
        }

        if (privateKey.isNull()) {
            qDebug() << "Private key is null. Check file path and format.";
            return false;
        }

        QSslConfiguration sslConfiguration;
        sslConfiguration.setLocalCertificate(certificate);
        sslConfiguration.setPrivateKey(privateKey);
        sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);

        server->sslSetup(sslConfiguration);

        // 监听 https
        if (!server->listen(QHostAddress::Any, httpsPort))
        {
            qDebug() << "Failed to listen on port!" << httpsPort;
            return false;
        }
        qDebug() << "Https server running on port." << httpsPort;
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
        rootDir.mkpath(QDir::separator());  // 创建根文件夹
    }
}
