#include "GetWebFileHandler.h"
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QFile>
#include <QMimeDatabase>
#include <RogalunaHttpConfig.h>
#include <RogalunaStorageServer.h>

QHttpServerResponse GetWebFileHandler::handleRequest(QUrl path, const QHttpServerRequest &request)
{
    Q_UNUSED(request)

    QString filePath = path.toString();

    // 空路径默认设置 index.html
    if (filePath.isEmpty()) {
        filePath = "index.html";
    }

    QString fullPath = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
        *RogalunaHttpConfig::getInstance().getWebRootPath() + QDir::separator() +
        filePath);
    qDebug() << "Trying to serve file: " << fullPath;  // Debug output

    QFile file(fullPath);

    // 如果文件存在，返回对应的文件
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray fileContent = file.readAll();

        // 使用 QMimeDatabase 自动识别 MIME 类型
        QMimeDatabase mimeDatabase;
        QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

        // qDebug() << "MIME Type: " << mimeType.name();

        return QHttpServerResponse(mimeType.name().toUtf8(), fileContent);
    }

    // 如果文件不存在，则返回 index.html
    else {
        QString indexPath = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
            *RogalunaHttpConfig::getInstance().getWebRootPath() + QDir::separator() + "index.html");

        qDebug() << "Serving fallback index.html for SPA route";

        QFile indexFile(indexPath);
        if (indexFile.exists() && indexFile.open(QIODevice::ReadOnly)) {
            QByteArray indexContent = indexFile.readAll();

            QMimeDatabase mimeDatabase;
            QMimeType mimeType = mimeDatabase.mimeTypeForFile("index.html");
            return QHttpServerResponse(mimeType.name().toUtf8(), indexContent);
        }
    }

    // 默认返回 404 Not Found
    return QHttpServerResponse("404 Not Found", QHttpServerResponse::StatusCode::NotFound);
}

QHttpServerResponse RedirectToWebHandler::handleRequest(const QHttpServerRequest &)
{
    QHttpServerResponse response(QHttpServerResponse::StatusCode::MovedPermanently);
    response.setHeader("Location", "/web/");
    return response;
}
