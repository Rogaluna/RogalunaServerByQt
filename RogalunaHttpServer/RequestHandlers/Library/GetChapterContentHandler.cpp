#include "GetChapterContentHandler.h"
#include <QHttpServerRequest>
#include <RogalunaHttpConfig.h>
#include <RogalunaLibraryServer.h>

#include <Macro/TokenGV.h>

namespace Library {


QHttpServerResponse GetChapterContentHandler::handleRequest(const QHttpServerRequest &request)
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

    QUrlQuery query = request.query();
    // 提取书籍 id 和章节 index
    QString bookId;
    QString chapterIndex;
    if (query.hasQueryItem("id") || query.hasQueryItem("index")) {
        bookId = query.queryItemValue("id");
        chapterIndex = query.queryItemValue("index");
    } else {
        // 如果没有 id 和 index 参数，返回错误响应
        QHttpServerResponse response("Missing id in query parameters", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    QByteArray fileData;
    qint64 startPos = 0;
    qint64 endPos = 0;
    qint64 fileSize = 0;
    bool isRangeRequest = false;
    QString chapterName;

    // 获取请求头，检查是否为 Range 请求（断点续传）
    QString rangeHeader;
    for (const auto &header : headers) {
        if (header.first.toLower() == "range") {
            rangeHeader = header.second;
            break;
        }
    }

    if (!rangeHeader.isEmpty()) {
        static QRegularExpression re("bytes=(\\d+)-(\\d*)");
        QRegularExpressionMatch match = re.match(rangeHeader);

        if (match.hasMatch()) {
            // 有断点续传请求
            isRangeRequest = true;
            startPos = match.captured(1).toLongLong();
            if (!match.captured(2).isEmpty()) {
                endPos = match.captured(2).toLongLong();
            } else {
                // 这里暂时不设置 endPos，因为还不知道文件大小
                // getTargetFile 会在 isRangeRequest 为 true 时处理 endPos
                // 确保 getTargetFile 能够处理 endPos 未指定的情况
                endPos = -1; // 使用 -1 作为未指定的标志
            }
        }
    }

    bool bSuccess = RogalunaHttpConfig::getInstance().getLibraryServer()->getChapterFile(bookId, chapterIndex.toInt(), isRangeRequest, fileData, startPos, endPos, fileSize, chapterName);

    if (!bSuccess) {
        QJsonObject jsonObject;
        jsonObject["success"] = false;
        jsonObject["message"] = isRangeRequest ? "Requested Range Not Satisfiable" : "File not found or cannot be opened";
        QJsonDocument jsonDoc(jsonObject);
        QByteArray jsonResponse = jsonDoc.toJson();

        QHttpServerResponse response("application/json", jsonResponse, isRangeRequest ? QHttpServerResponse::StatusCode::RequestRangeNotSatisfiable : QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    // 设置响应头
    QHttpServerResponse response(fileData, isRangeRequest ? QHttpServerResponse::StatusCode::PartialContent : QHttpServerResponse::StatusCode::Ok);
    if (isRangeRequest) {
        // 206 Partial Content 响应
        response.setHeader("Content-Range", QString("bytes %1-%2/%3").arg(startPos, endPos, fileSize).toUtf8());
    }

    response.setHeader("Content-Type", "application/octet-stream");
    response.setHeader("Content-Length", QString::number(fileData.size()).toUtf8());
    response.setHeader("Content-Disposition", QString("filename=\"%1\"").arg(chapterName).toUtf8());
    response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域

    return response;

    // // 返回 JSON 响应
    // QJsonObject jsonResponse;
    // jsonResponse["status"] = "success";
    // jsonResponse["data"] = result;

    // QJsonDocument jsonDoc(jsonResponse);
    // QByteArray jsonResponseData = jsonDoc.toJson();

    // // 返回带有 CORS 头的 JSON 响应
    // QHttpServerResponse response("application/json", jsonResponseData);
    // response.setHeader("Content-Length", QString::number(fileData.size()).toUtf8());
    // response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    // return response;
}

}
