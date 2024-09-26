#include "FetchFileDirectLinkHandler.h"

#include <QHttpServerRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <RogalunaHttpConfig.h>
#include <Macro/RequestBodyParser.h>
#include <Macro/TokenGV.h>

namespace CloudDrive {

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

    QString targetMd5;
    QString fileName;
    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "targetMd5") {
            targetMd5 = QString::fromUtf8(part.data).trimmed();
            targetMd5 = QUrl::fromPercentEncoding(targetMd5.toUtf8());
        } else if (part.name == "fileName") {
            fileName = QString::fromUtf8(part.data).trimmed();
        }
    }

    if (targetMd5.isEmpty() || fileName.isEmpty()) {
        QHttpServerResponse response("Missing metadata", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // 检查 targetMd5 是否合法


    // 使用安全令牌生成下载 token，使用秘密密钥加密保证只能用本服务器解析才能得到目标文件，确保 URL 只有短期有效性
    QList<QPair<QString, QString>> claims = {
        {"targetMd5", targetMd5},
        {"fileName", fileName},
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

}
