#include "MergeFileHandler.h"

#include <QHttpServerRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <RogalunaHttpConfig.h>
#include <RogalunaStorageServer.h>
#include <RogalunaCloudDriveServer.h>

#include <Macro/RequestBodyParser.h>
#include <Macro/TokenGV.h>

QHttpServerResponse MergeFileHandler::handleRequest(const QHttpServerRequest &request)
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

    // 解析请求体（FormData）
    QByteArray bodyParts = request.body();

    // 解析表单数据
    QString uid;
    QString parentUid;
    QString fileName;
    qint64 totalChunks = -1;

    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "uid") {
            uid = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "parentUid") {
            parentUid = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "fileName") {
            fileName = QUrl::fromPercentEncoding(part.data).trimmed();
        } else if (part.name == "totalChunks") {
            totalChunks = part.data.toLongLong();
        }
    }

    // 检查是否收到了必须的参数
    if (uid.isEmpty() || fileName.isEmpty() || totalChunks == -1) {
        QHttpServerResponse response("Missing merge metadata", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    if(!RogalunaHttpConfig::getInstance().getCloudDriveServer()->mergeChunks(uid, fileName, totalChunks, userId.toInt(), parentUid)) {
        QHttpServerResponse response("merge chunk Fail!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // 合并成功，返回响应
    QJsonObject jsonResponse;
    jsonResponse["status"] = "success";

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}
