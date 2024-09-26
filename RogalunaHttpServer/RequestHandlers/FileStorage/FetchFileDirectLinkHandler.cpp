#include "FetchFileDirectLinkHandler.h"

#include <QHttpServerRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <RogalunaHttpConfig.h>
#include <Macro/RequestBodyParser.h>
#include <Macro/TokenGV.h>

QHttpServerResponse FetchFileDirectLinkHandler::handleRequest(const QHttpServerRequest &request)
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

    // 遍历头部列表，检查是否为 multipart form-data 类型
    QString contentType;
    for (const QPair<QByteArray, QByteArray> &header : headers) {
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

    // 获取请求体中的表单数据
    QByteArray bodyParts = request.body();

    // 解析表单数据的结构
    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    QString filePath;
    for (const MultipartPart &part : formParts) {
        if (part.name == "filePath") {
            filePath = QString::fromUtf8(part.data).trimmed();
            break;
        }
    }

    if (filePath.isEmpty()) {
        QHttpServerResponse response("Missing file parameter", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    filePath = QUrl::fromPercentEncoding(filePath.toUtf8());
    // 检查 filePath 是否以 '/' 开头，如果是则移除它
    if (filePath.startsWith('/')) {
        filePath.remove(0, 1);
    }

    // 使用安全令牌生成下载 URL，确保 URL 只有短期有效性
    QList<QPair<QString, QString>> claims = {
        {"filePath", filePath},
        {"iat", QString::number(QDateTime::currentDateTime().toSecsSinceEpoch())},
        {"exp", QString::number(QDateTime::currentDateTime().addSecs(3600).toSecsSinceEpoch())}
    };
    QString token = GENERATE_JWT_TOKEN(claims, *RogalunaHttpConfig::getInstance().getAlgorithm(), *RogalunaHttpConfig::getInstance().getSecretKey());

    QJsonObject jsonObject;
    jsonObject["downloadToken"] = token;
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonResponse = jsonDoc.toJson();

    QHttpServerResponse response("application/json", jsonResponse);
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}
