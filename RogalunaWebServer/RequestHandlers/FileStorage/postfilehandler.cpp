#include "postfilehandler.h"
#include <rogalunastorageserver.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

PostFileHandler::PostFileHandler(RogalunaStorageServer *storageServer)
    : storageServer(storageServer)
{}

void PostFileHandler::handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data)
{
    Q_UNUSED(path);
    Q_UNUSED(query);

    // 解析请求头
    QMap<QString, QString> headers = parseHeaders(parseRequest(data).first);
    // 从头中获取文件（块）信息
    QString fileName = QUrl::fromPercentEncoding(headers.value("x-file-name").toUtf8());
    QString chunkIndex = headers.value("x-chunk-index");
    // QString totalChunks = headers.value("x-total-chunks");
    QString targetPath = QUrl::fromPercentEncoding(headers.value("x-target-path").toUtf8());
    QString mergeRequest = headers.value("x-merge-request");

    QByteArray boundary = parseBoundary(headers);

    if (!boundary.isEmpty()) {
        QList<MultipartPart> bodyParts = parseBody(parseRequest(data).second, boundary);

        QByteArray fileContent;

        // 遍历解析的部分，找到文件和目标路径
        for (const MultipartPart &part : bodyParts) {
            QString contentDisposition = part.headers.value("content-disposition");

            // 找到 file 字段
            if (contentDisposition.contains("name=\"file\"")) {
                fileContent = part.content;
            }

        }

        // 检查是否获取到了文件内容
        if (fileContent.isEmpty()) {
            sendErrorResponse(socket, "Missing file content or target path.");
            return;
        }

        QString finalFileName = fileName + ".ROGALUNALOADING";
        QString filePath = storageServer->absoluteFilePath(targetPath.isEmpty() ? finalFileName : targetPath + "/" + finalFileName);

        // 如果是第一个块则创建文件，否则打开文件以追加内容
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | (chunkIndex.toInt() > 0 ? QIODevice::Append : QIODevice::Truncate))) {
            sendErrorResponse(socket, "Failed to open target file for writing.");
            return;
        }

        // 写入文件块
        if (file.write(fileContent) == -1) {
            sendErrorResponse(socket, "Failed to write file content.");
            return;
        }
        file.close();

        // 如果收到 mergeRequest 并为 true，说明文件传输完成
        if (mergeRequest == "true") {
            QString finalPath = storageServer->absoluteFilePath(targetPath.isEmpty() ? fileName : targetPath + "/" + fileName);
            if (QFile::exists(finalPath)) {
                QFile::remove(finalPath);
            }
            if (QFile::rename(filePath, finalPath)) {
                sendResponse(socket, "File uploaded and merged successfully.", "text/plain");
            } else {
                sendErrorResponse(socket, "Failed to finalize the file.");
            }
        } else {
            sendResponse(socket, "Chunk uploaded successfully.", "text/plain");
        }

        file.close();
    } else {
        sendErrorResponse(socket, "Invalid multipart/form-data boundary.");
    }
}


