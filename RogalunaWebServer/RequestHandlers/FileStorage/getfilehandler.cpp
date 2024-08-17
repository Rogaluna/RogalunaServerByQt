#include "getfilehandler.h"
#include <RogalunaStorageServer.h>
#include <QJsonObject>
#include <QJsonDocument>

GetFileHandler::GetFileHandler(RogalunaStorageServer *storageServer)
    : storageServer(storageServer)
{}

void GetFileHandler::handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data)
{
    Q_UNUSED(data);
    Q_UNUSED(path);

    QString filePath = "";

    // 如果查询参数 "file" 存在，则使用其值作为文件路径
    if (query.hasQueryItem("file")) {
        filePath = query.queryItemValue("file");
        filePath = QUrl::fromPercentEncoding(filePath.toUtf8());

        // 检查 filePath 是否以 '/' 开头，如果是则移除它
        if (filePath.startsWith('/')) {
            filePath.remove(0, 1);
        }
    }

    QFile file(storageServer->absoluteFilePath(filePath));
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        QJsonObject jsonObject;
        jsonObject["error"] = "File not found or cannot be opened";
        QJsonDocument jsonDoc(jsonObject);
        QByteArray jsonResponse = jsonDoc.toJson();
        sendResponse(socket, jsonResponse, "application/json");
        return;
    }

    // 获取文件大小
    qint64 fileSize = file.size();

    // 设置 HTTP 响应头
    QString responseHeader = QString("HTTP/1.1 200 OK\r\n"
                                     "content-type: application/octet-stream\r\n"
                                     "content-length: %1\r\n"
                                     "connection: close\r\n\r\n")
                                 .arg(fileSize);

    socket->write(responseHeader.toUtf8());

    // 分块读取和发送文件内容
    while (true) {
        FileReadResult readResult = storageServer->readFile(file);
        if (!readResult.success) {
            if (!readResult.atEnd) {
                QJsonObject jsonObject;
                jsonObject["error"] = readResult.error;
                QJsonDocument jsonDoc(jsonObject);
                QByteArray jsonResponse = jsonDoc.toJson();
                sendResponse(socket, jsonResponse, "application/json");
            }
            break;
        }

        socket->write(readResult.data);
        socket->waitForBytesWritten();

        if (readResult.atEnd) {
            break;
        }
    }

    file.close();
}
