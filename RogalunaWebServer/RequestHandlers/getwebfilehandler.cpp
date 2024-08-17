#include "getwebfilehandler.h"

#include <QFile>

GetWebFileHandler::GetWebFileHandler(const QString &rootPath)
    : webRootPath(rootPath)
{}

void GetWebFileHandler::handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data)
{
    Q_UNUSED(query);
    Q_UNUSED(data);

    QString filePath = path;
    if (filePath.startsWith("/")) {
        filePath = filePath.mid(1);
    }
    if (filePath.isEmpty()) {
        filePath = "index.html";
    }

    QString fullPath = webRootPath + "/" + filePath;
    qDebug() << "Trying to serve file: " << fullPath;  // Debug output

    QFile file(fullPath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray content = file.readAll();
        QString contentType = getContentType(filePath);
        sendResponse(socket, content, contentType);
    } else {
        qDebug() << "File not found: " << fullPath;  // Debug output
        sendErrorResponse(socket, "404 Not Found");
    }
}

QString GetWebFileHandler::getContentType(const QString &filePath)
{
    if (filePath.endsWith(".html") || filePath.endsWith(".htm")) {
        return "text/html";
    } else if (filePath.endsWith(".css")) {
        return "text/css";
    } else if (filePath.endsWith(".js")) {
        return "application/javascript";
    } else if (filePath.endsWith(".png")) {
        return "image/png";
    } else if (filePath.endsWith(".jpg") || filePath.endsWith(".jpeg")) {
        return "image/jpeg";
    } else if (filePath.endsWith(".gif")) {
        return "image/gif";
    } else if (filePath.endsWith(".svg")) {
        return "image/svg+xml";
    } else if (filePath.endsWith(".json")) {
        return "application/json";
    } else {
        return "application/octet-stream";
    }
}
