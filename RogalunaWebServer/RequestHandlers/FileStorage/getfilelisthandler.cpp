#include "getfilelisthandler.h"
#include <RogalunaStorageServer.h>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QFileInfo>

GetFileListHandler::GetFileListHandler(RogalunaStorageServer *storageServer)
    : storageServer(storageServer)
{}

void GetFileListHandler::handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data)
{
    Q_UNUSED(data);
    Q_UNUSED(path);

    QString dirPath = "";

    // 如果查询参数 "directory" 存在，则使用其值作为目录路径
    if (query.hasQueryItem("directory")) {
        dirPath = query.queryItemValue("directory");
        dirPath = QUrl::fromPercentEncoding(dirPath.toUtf8());
    }

    QVector<FileInfoStruct> entries = storageServer->listFiles(dirPath);

    QJsonArray jsonArray;
    for (const FileInfoStruct &entry : entries) {
        QJsonObject jsonObject;
        jsonObject["name"] = entry.name;
        jsonObject["type"] = entry.type;
        jsonObject["size"] = entry.size;
        jsonObject["lastModified"] = entry.lastModified.toString(Qt::ISODate);

        jsonArray.append(jsonObject);
    }

    QJsonDocument jsonDoc(jsonArray);
    QByteArray jsonResponse = jsonDoc.toJson();

    sendResponse(socket, jsonResponse, "application/json");
}
