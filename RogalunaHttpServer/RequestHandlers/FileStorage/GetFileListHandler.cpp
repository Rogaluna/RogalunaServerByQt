#include "GetFileListHandler.h"

#include <QHttpServerRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <RogalunaHttpConfig.h>
#include <RogalunaCloudDriveServer.h>

#include <Macro/TokenGV.h>

QHttpServerResponse GetFileListHandler::handleRequest(const QHttpServerRequest &request)
{
    // 遍历头部列表，查找 "Authorization" 头
    QList<QPair<QByteArray, QByteArray>> headers = request.headers();
    QByteArray authorizationValue;
    for (const auto &header : headers) {
        if (header.first.toLower() == "authorization") {
            authorizationValue = header.second;
            break;
        }
    }

    auto [isValid, header, payload] = VERIFY_JWT_TOKEN(QString(authorizationValue), *RogalunaHttpConfig::getInstance().getSecretKey());
    // 令牌验证非法
    if (!isValid) {
        QHttpServerResponse response("Invalid token", QHttpServerResponse::StatusCode::Unauthorized);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    // 令牌验证通过，获取负载对象
    QJsonDocument jwtDoc = QJsonDocument::fromJson(payload.toUtf8());
    QJsonObject jwtObj = jwtDoc.object();

    // 验证时间
    int checkResult = CHECK_TIMESTAMPS(jwtObj);
    switch (checkResult) {
    case 0:
    {
        QHttpServerResponse response("Token Error: missing 'iat' or 'exp' claim.", QHttpServerResponse::StatusCode::Unauthorized);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }
    case 1:
    {
        QHttpServerResponse response("Token Error: iat is in the future.", QHttpServerResponse::StatusCode::Unauthorized);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }
    case 2:
    {
        QHttpServerResponse response("Token has expired!", QHttpServerResponse::StatusCode::Unauthorized);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }
    case 3:
        // 验证通过
        break;
    }

    // 获取负载数据中包含的用户信息
    QString userId = jwtObj.value("id").toString();

    QUrlQuery query(request.url());
    QString parentFolderUid = "";

    // 如果查询参数 "directory" 存在，则使用其值作为目录路径
    if (query.hasQueryItem("directory")) {
        parentFolderUid = query.queryItemValue("directory");
        parentFolderUid = QUrl::fromPercentEncoding(parentFolderUid.toUtf8());
    }

    // 如果是空值，那么获取用户的根目录 uid
    if (parentFolderUid.isEmpty()) {
        parentFolderUid = RogalunaHttpConfig::getInstance().getCloudDriveServer()->getUserRootDirUid(userId.toInt());
    }

    // 调用存储服务器获取文件列表
    QVector<FileMetadata> entries = RogalunaHttpConfig::getInstance().getCloudDriveServer()->getFiles(
                                       parentFolderUid,
                                       RogalunaCloudDriveServer::EGetFileOpterator::E_FOLDER).value_or(QVector<FileMetadata>());

    // 构建JSON响应
    QJsonArray jsonArray;
    for (const FileMetadata &entry : entries) {
        QJsonObject jsonObject;
        jsonObject["name"] = entry.fileName;
        jsonObject["contentMd5"] = entry.contentMd5;
        jsonObject["lastModified"] = entry.updatedAt.toString(Qt::ISODate);
        jsonObject["type"] = (entry.isDirectory ? "dir" : (entry.fileName.lastIndexOf('.') != -1 ? entry.fileName.mid(entry.fileName.lastIndexOf('.') + 1) : ""));
        // jsonObject["size"] = entry.size;

        jsonArray.append(jsonObject);
    }

    QJsonObject responseObject;
    responseObject["success"] = true;
    responseObject["currentFolderUid"] = parentFolderUid;
    responseObject["data"] = jsonArray;
    QJsonDocument jsonDoc(responseObject);
    QByteArray jsonResponse = jsonDoc.toJson();

    // 返回带有 JSON 数据的 QHttpServerResponse
    QHttpServerResponse response("application/json", jsonResponse);
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}
