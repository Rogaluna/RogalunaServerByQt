#include "ModifyAccountInfoHandler.h"

#include <QHttpServerRequest>
#include <RogalunaHttpConfig.h>
#include <RogalunaAccountServer.h>

#include <Macro/TokenGV.h>
#include <Macro/RequestBodyParser.h>

namespace Account {

QHttpServerResponse ModifyAccountInfoHandler::handleRequest(const QHttpServerRequest &request)
{
    QList<QPair<QByteArray, QByteArray>> headers = request.headers();

    // 遍历头部列表，查找 "Authorization" 头
    QByteArray authorizationValue;
    for (const QPair<QByteArray, QByteArray> &header : headers) {
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

    // 检查是否为 multipart form-data 类型
    QString contentType;
    for (const auto &header : headers) {
        if (header.first.toLower() == "content-type") {
            contentType = header.second;
            break;
        }
    }

    // 如果不是 multipart/form-data 请求，返回错误
    if (!contentType.startsWith("multipart/form-data")) {
        QHttpServerResponse response(QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    // 获取请求头中的数据
    QString targetId;
    QString username;
    QString password;
    QString oldPassword;

    // 获取请求体（FormData）中的数据
    QByteArray bodyParts = request.body();

    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "userId") {
            targetId = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "username") {
            username = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "password") {
            password = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "oldPassword") {
            oldPassword = QString::fromUtf8(part.data).trimmed();
        }
    }

    // 检查是否收到了必须的参数
    if (targetId.isEmpty()) {
        // 如果没有传入 targetId ，那么使用 userId 替代，并进行验证旧密码是否正确
        targetId = userId;

        if (!RogalunaHttpConfig::getInstance().getAccountServer()->verifyPassword(userId.toInt(), oldPassword)) {
            // 旧密码错误，不是本人进行操作
            QHttpServerResponse response("Password error, not the owner!", QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");
            return response;
        }
    } else {
        // 如果传入了 targetId ，那么判定用户是否有权限进行修改，检查 authority

        // 查询 userId 的权限
        QString authoriry;

        if (authoriry != "admin") {
            // 用户权限不是管理员
            QHttpServerResponse response("No permission", QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");
            return response;
        }
    }

    if (username.isEmpty() || password.isEmpty()) {
        QHttpServerResponse response("Missing metadata", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // 更新账户数据
    bool bSuccess = RogalunaHttpConfig::getInstance().getAccountServer()->updateAccountInfo(
            targetId,
            username,
            password
        );

    // 返回 JSON 响应
    QJsonObject jsonResponse;
    jsonResponse["success"] = bSuccess;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}

}
