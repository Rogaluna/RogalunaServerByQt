#include "UploadChapterResourceHandler.h"

#include <QHttpServerRequest>
#include <RogalunaHttpConfig.h>
#include <RogalunaLibraryServer.h>

#include <Macro/TokenGV.h>
#include <Macro/RequestBodyParser.h>

namespace Library {

QHttpServerResponse UploadChapterResourceHandler::handleRequest(const QHttpServerRequest &request)
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

    // 获取请求头中的元数据（如文件名、块索引、总块数等）
    QString bookId;
    QString chapterIndex;
    QString type;
    QByteArray data;
    QString md5;

    // 获取请求体（FormData）中的数据
    QByteArray bodyParts = request.body();

    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "id") {
            bookId = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "index") {
            chapterIndex = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "type") {
            type = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "data") {
            data = part.data;
        } else if (part.name == "md5") {
            md5 = QString::fromUtf8(part.data).trimmed();
        }
    }

    // 检查是否收到了必须的参数
    if (bookId.isEmpty() || chapterIndex.isEmpty() || (chapterIndex.toInt() < 0) || type.isEmpty() || data.isEmpty() || md5.isEmpty()) {
        QHttpServerResponse response("Missing upload metadata", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    const QJsonObject &result = RogalunaHttpConfig::getInstance().getLibraryServer()->getChapterInfo(bookId, chapterIndex);
    const QString &chapterName = result["file_name"].toString();

    bool bSuccess = RogalunaHttpConfig::getInstance().getLibraryServer()->uplaodChapterResource(bookId, chapterName, md5, type, data, md5);
    if (!bSuccess) {
        QHttpServerResponse response("Fail save resource!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    QList<QPair<QString, QString>> claims = {
        {"id", bookId},
        {"name", chapterName},
        {"md5", md5},
    };
    QString resExtractToken = GENERATE_JWT_TOKEN(claims, *RogalunaHttpConfig::getInstance().getAlgorithm(), *RogalunaHttpConfig::getInstance().getSecretKey());

    // 返回 JSON 响应
    QJsonObject jsonResponse;
    jsonResponse["success"] = bSuccess;
    jsonResponse["data"] = resExtractToken;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}

}
