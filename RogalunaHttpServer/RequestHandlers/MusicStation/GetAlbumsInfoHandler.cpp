#include "GetAlbumsInfoHandler.h"

#include <QHttpServerRequest>
#include <QJsonArray>
#include <RogalunaHttpConfig.h>
#include <RogalunaMusicServer.h>

#include <Macro/TokenGV.h>

namespace MusicStation {

QHttpServerResponse GetAlbumsInfoHandler::handleRequest(const QHttpServerRequest &request)
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

    QUrlQuery query = request.query();
    // 提取 id
    QStringList ids;
    if (query.hasQueryItem("id")) {
        ids.append(query.queryItemValue("id"));
    } else {
        // 如果没有 id 参数，返回错误响应
        QHttpServerResponse response("Missing query parameters", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    QJsonArray result = RogalunaHttpConfig::getInstance().getMusicServer()->getAlbumsMetadata(ids);

    if (result.isEmpty()) {
        // 如果获取的结果是空的
        QHttpServerResponse response("Empty data!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    // 返回 JSON 响应
    QJsonObject jsonResponse;
    jsonResponse["success"] = true;
    jsonResponse["data"] = result;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}

}
