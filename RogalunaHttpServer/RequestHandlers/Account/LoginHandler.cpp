#include "LoginHandler.h"

#include <QHttpServerRequest>
#include <RogalunaHttpConfig.h>
#include <RogalunaAccountServer.h>

#include <Macro/RequestBodyParser.h>
#include <Macro/TokenGV.h>

namespace Account {

QHttpServerResponse LoginHandler::handleRequest(const QHttpServerRequest &request)
{
    // 检查是否为 multipart form-data 类型
    QList<QPair<QByteArray, QByteArray>> headers = request.headers();
    QString contentType;
    for (const auto &header : headers) {
        if (header.first.toLower() == "content-type") {
            contentType = header.second;
            break;
        }
    }

    if (!contentType.startsWith("multipart/form-data")) {
        QHttpServerResponse response(QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    // 解析请求体（FormData）
    QByteArray bodyParts = request.body();

    // 解析表单数据
    QString usernameOrId;
    QString password;

    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "usernameOrId") {
            usernameOrId = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "password") {
            password = QString::fromUtf8(part.data).trimmed();
        }
    }

    QJsonObject result = RogalunaHttpConfig::getInstance().getAccountServer()->loginAccount(usernameOrId, password);

    if (result["success"].toBool() == false) {
        QJsonDocument jsonDoc(result);
        QByteArray jsonResponseData = jsonDoc.toJson();

        QHttpServerResponse response("application/json", jsonResponseData, QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    const QJsonObject &user = result["data"].toObject();

    // 密码验证通过, 生成 Token
    QList<QPair<QString, QString>> claims = {
        {"id", QString::number(user["userId"].toInt())},
        {"username", user["username"].toString()},
        {"authority", user["authority"].toString()},
        {"iat", QString::number(QDateTime::currentDateTime().toSecsSinceEpoch())},
        {"exp", QString::number(QDateTime::currentDateTime().addDays(7).toSecsSinceEpoch())}
    };
    QString jwtToken = GENERATE_JWT_TOKEN(claims, *RogalunaHttpConfig::getInstance().getAlgorithm(), *RogalunaHttpConfig::getInstance().getSecretKey());

    // 登录成功，返回 JSON 响应
    QJsonObject jsonResponse;
    jsonResponse["token"] = jwtToken;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}

}
