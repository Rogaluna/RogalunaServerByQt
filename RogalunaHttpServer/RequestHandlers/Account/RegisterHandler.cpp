#include "RegisterHandler.h"

#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <RogalunaHttpConfig.h>
#include <RogalunaDatabaseServer.h>

#include <Macro/RequestBodyParser.h>
#include <bcrypt.h>

#include <Database/Account/UsersDAO.h>

QHttpServerResponse RegisterHandler::handleRequest(const QHttpServerRequest &request)
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
    QString username;
    QString password;

    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "username") {
            username = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "password") {
            password = QString::fromUtf8(part.data).trimmed();
        }
    }

    // bcrypt 进行加密
    int saltRounds = 10;
    QString hashedPassword = QString::fromStdString(bcrypt::generateHash(password.toStdString(), saltRounds));

    UserDAO userDAO(RogalunaHttpConfig::getInstance().getDatabaseServer()->getDatabase());
    std::optional<User> newUser = userDAO.registerUser(username, hashedPassword);
    if (!newUser) {
        // 注册失败
        QHttpServerResponse response("Register Fail!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    // 返回 JSON 响应，表示注册成功
    QJsonObject jsonResponse;
    jsonResponse["id"] = newUser->id;
    jsonResponse["username"] = newUser->username;
    jsonResponse["authority"] = newUser->authority;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}
