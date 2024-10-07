#include "GetMusicListHandler.h"

#include <QHttpServerRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QHttpServerResponse GetMusicListHandler::handleRequest(const QHttpServerRequest &request)
{
    Q_UNUSED(request)

    // 构建JSON响应
    QJsonArray jsonArray;
    // for (const FileMetadata &entry : entries) {
    //     QJsonObject jsonObject;
    //     jsonObject["uid"] = entry.uid;
    //     jsonObject["name"] = entry.fileName;
    //     jsonObject["contentMd5"] = entry.contentMd5;
    //     jsonObject["lastModified"] = entry.updatedAt.toString(Qt::ISODate);
    //     jsonObject["type"] = (entry.isDirectory ? "dir" : (entry.fileName.lastIndexOf('.') != -1 ? entry.fileName.mid(entry.fileName.lastIndexOf('.') + 1) : ""));
    //     // jsonObject["size"] = entry.size;

    //     jsonArray.append(jsonObject);
    // }

    QJsonObject responseObject;
    responseObject["success"] = true;
    responseObject["data"] = jsonArray;
    QJsonDocument jsonDoc(responseObject);
    QByteArray jsonResponse = jsonDoc.toJson();

    // 返回带有 JSON 数据的 QHttpServerResponse
    QHttpServerResponse response("application/json", jsonResponse);
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}
