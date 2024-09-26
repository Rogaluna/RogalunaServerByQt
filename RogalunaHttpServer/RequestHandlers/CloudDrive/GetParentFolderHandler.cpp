#include "GetParentFolderHandler.h"

#include <QHttpServerRequest>
#include <RogalunaHttpConfig.h>
#include <RogalunaCloudDriveServer.h>

#include <Macro/TokenGV.h>

namespace CloudDrive {

QHttpServerResponse GetParentFolderHandler::handleRequest(const QHttpServerRequest &request)
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
    QString folderUid = "";

    // 如果查询参数 "directory" 存在
    if (query.hasQueryItem("directory")) {
        folderUid = query.queryItemValue("directory");
        folderUid = QUrl::fromPercentEncoding(folderUid.toUtf8());
    }

    // 如果是空值，报错
    if (folderUid.isEmpty()) {
        QHttpServerResponse response("Search param not exist!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    const std::optional<CloudDrive::FFileMetadata> &searchResult = RogalunaHttpConfig::getInstance().getCloudDriveServer()->getParent(folderUid);

    // 如果没有找到值，报错
    if (!searchResult.has_value()) {
        QHttpServerResponse response("Failed search!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // 构建JSON响应
    QJsonObject responseObject;
    responseObject["success"] = true;
    responseObject["parentUid"] = searchResult->parentUid;

    QJsonDocument jsonDoc(responseObject);
    QByteArray jsonResponse = jsonDoc.toJson();

    QHttpServerResponse response("application/json", jsonResponse);
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}

}
