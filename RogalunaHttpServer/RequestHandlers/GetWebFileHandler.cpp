#include "GetWebFileHandler.h"
#include <QHttpServerRequest>
#include <QFile>
#include <QMimeDatabase>
#include <RogalunaHttpConfig.h>

QHttpServerResponse GetWebFileHandler::handleRequest(QUrl path, const QHttpServerRequest &request)
{
    Q_UNUSED(request)

    QString filePath = path.toString();

    if (filePath.isEmpty()) {
        filePath = "index.html";
    }

    QString fullPath = *RogalunaHttpConfig::getInstance().getWebRootPath() + "/" + filePath;
    qDebug() << "Trying to serve file: " << fullPath;  // Debug output

    QFile file(fullPath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray fileContent = file.readAll();

        // 使用 QMimeDatabase 自动识别 MIME 类型
        QMimeDatabase mimeDatabase;
        QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

        qDebug() << "MIME Type: " << mimeType.name();

        return QHttpServerResponse(mimeType.name().toUtf8(), fileContent);
    } else {
        return QHttpServerResponse("404 Not Found", QHttpServerResponse::StatusCode::NotFound);
    }
}
