#include "LoginHandler.h"

#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <RogalunaHttpConfig.h>
#include <RogalunaDatabaseServer.h>

#include <Macro/RequestBodyParser.h>
#include <Macro/TokenGV.h>

#include <bcrypt.h>

#include <Database/Account/UsersDAO.h>

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

    UserDAO userDAO(RogalunaHttpConfig::getInstance().getDatabaseServer()->getDatabase());
    std::optional<User> user = std::nullopt;
    bool isId;
    int id = usernameOrId.toInt(&isId);
    if (isId) {
        user = userDAO.getUserById(id);
    } else {
        user = userDAO.getUserByName(usernameOrId);
    }

    // 未检索到账户
    if (user == std::nullopt) {
        QHttpServerResponse response("Account does not exist", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    // 获取了加密密码，使用 bcrypt 验证密码
    bool isPasswordCorrect = bcrypt::validatePassword(password.toStdString(), user->password.toStdString());

    // 密码错误
    if (!isPasswordCorrect) {
        QHttpServerResponse response("Password Incorrect!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    // 密码验证通过, 生成 Token
    QList<QPair<QString, QString>> claims = {
        {"id", QString::number(user->id)},
        {"username", user->username},
        {"authority", user->authority},
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
