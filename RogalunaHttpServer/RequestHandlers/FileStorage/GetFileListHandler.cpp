#include "GetFileListHandler.h"

#include <QHttpServerRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <RogalunaHttpConfig.h>
#include <RogalunaStorageServer.h>

QHttpServerResponse GetFileListHandler::handleRequest(const QHttpServerRequest &request)
{
    QUrlQuery query(request.url());

    QString dirPath = "";

    // 如果查询参数 "directory" 存在，则使用其值作为目录路径
    if (query.hasQueryItem("directory")) {
        dirPath = query.queryItemValue("directory");
        dirPath = QUrl::fromPercentEncoding(dirPath.toUtf8());
    }

    // 调用存储服务器获取文件列表
    QVector<FileInfoStruct> entries = RogalunaHttpConfig::getInstance().getStorageServer()->listFiles(dirPath);

    // 构建JSON响应
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

    // 返回带有 JSON 数据的 QHttpServerResponse
    QHttpServerResponse response("application/json", jsonResponse);
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}
