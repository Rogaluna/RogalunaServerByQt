#include "PostFileHandler.h"

#include <QFile>
#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <RogalunaHttpConfig.h>
#include <RogalunaStorageServer.h>
#include <RogalunaCloudDriveServer.h>

#include <Macro/RequestBodyParser.h>
#include <Macro/TokenGV.h>

QHttpServerResponse PostFileHandler::handleRequest(const QHttpServerRequest &request)
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

    // 获取请求头中的元数据（如文件名、块索引、总块数等）
    QString uid;
    qint64 chunkIndex = -1;
    qint64 totalChunks = -1;
    QByteArray chunkData;
    QString chunkMd5;

    // 获取请求体（FormData）中的数据
    QByteArray bodyParts = request.body();

    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "uid") {
            uid = QString::fromUtf8(part.data).trimmed(); // 这获取的 uid 即是上传的文件的 Md5 值
        } else if (part.name == "chunkIndex") {
            chunkIndex = part.data.toLongLong();
        } else if (part.name == "totalChunks") {
            totalChunks = part.data.toLongLong();
        } else if (part.name == "chunkData") {
            chunkData = part.data;
        } else if (part.name == "chunkMd5") {
            chunkMd5 = QString::fromUtf8(part.data).trimmed();
        }
    }

    // 检查是否收到了必须的参数
    if (uid.isEmpty() || chunkData.isEmpty() || chunkMd5.isEmpty() || chunkIndex == -1 || totalChunks == -1) {
        QHttpServerResponse response("Missing upload metadata", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    if(!RogalunaHttpConfig::getInstance().getCloudDriveServer()->uploadChunk(uid, chunkIndex, chunkData, chunkMd5)) {
        QHttpServerResponse response("upload chunk Fail!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // 返回 JSON 响应，表示该块上传成功
    QJsonObject jsonResponse;
    jsonResponse["status"] = "success";
    jsonResponse["uploadedChunkIndex"] = chunkIndex;
    jsonResponse["totalChunks"] = totalChunks;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}


