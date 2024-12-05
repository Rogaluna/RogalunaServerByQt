#include "GetUidFromPathHandler.h"

#include <QHttpServerRequest>
#include <QUrlQuery>
#include <RogalunaHttpConfig.h>
#include <RogalunaCloudDriveServer.h>

#include <Macro/TokenGV.h>

namespace CloudDrive {

QHttpServerResponse GetUidFromPathHandler::handleRequest(const QHttpServerRequest &request)
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

    QString resultUid = "";

    QUrlQuery query(request.url());
    QString path = "";
    if (query.hasQueryItem("path")) {
        // 如果查询参数 "path" 存在
        path = query.queryItemValue("path");
        path = QUrl::fromPercentEncoding(path.toUtf8());

        // 检查是否以 "root" 开头
        if (path.startsWith("root")) {
            // 去掉 "root"
            path.remove(0, 4); // 删除 "root" 部分
        } else {
            // 报错查询参数错误
            QHttpServerResponse response("Query param error!", QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");
            return response;
        }

        // 获取路径指向的目录，如果找到了目标对象，并且目标对象是一个目录
        const std::optional<CloudDrive::FFileMetadata> &targetDir = RogalunaHttpConfig::getInstance().getCloudDriveServer()->getMetadataFromPath(path, userId.toInt());
        if (targetDir.has_value() && targetDir->isDirectory == true) {
            // 将 param 设置为目标对象的 uid
            resultUid = targetDir->uid;
        } else {
            // 报错
            QHttpServerResponse response("Failed search path!", QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");
            return response;
        }
    }

    // 返回 JSON 响应，表示该块上传成功
    QJsonObject jsonResponse;
    jsonResponse["status"] = "success";
    jsonResponse["uid"] = resultUid;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}

}
