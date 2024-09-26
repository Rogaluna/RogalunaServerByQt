#include "GetFileHandler.h"

#include <QHttpServerRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <RogalunaHttpConfig.h>
#include <RogalunaStorageServer.h>
#include <Macro/TokenGV.h>

QHttpServerResponse GetFileHandler::handleRequest(const QHttpServerRequest &request)
{
    // 从请求的 URL 获取查询参数
    QUrlQuery query = request.query();
    QList<QPair<QByteArray, QByteArray>> headers = request.headers();

    // 提取 token 参数
    QString token;
    if (query.hasQueryItem("token")) {
        token = query.queryItemValue("token");
    } else {
        // 如果没有 token 参数，返回错误响应
        QHttpServerResponse response("Missing token in query parameters", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    auto [isValid, header, payload] = VERIFY_JWT_TOKEN(token, *RogalunaHttpConfig::getInstance().getSecretKey());

    // 令牌验证非法
    if (!isValid) {
        QHttpServerResponse response("Invalid token", QHttpServerResponse::StatusCode::Unauthorized);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    // 令牌验证通过，获取负载对象
    QJsonDocument jsonDoc = QJsonDocument::fromJson(payload.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();

    // 验证时间
    int checkResult = CHECK_TIMESTAMPS(jsonObj);
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

    // 获取负载数据中包含的目标文件路径
    QString filePath = jsonObj.value("filePath").toString();

    // 打开文件
    QFile file(RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(filePath));
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        QJsonObject jsonObject;
        jsonObject["error"] = "File not found or cannot be opened";
        QJsonDocument jsonDoc(jsonObject);
        QByteArray jsonResponse = jsonDoc.toJson();

        QHttpServerResponse response("application/json", jsonResponse, QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    QFileInfo fileInfo(file);
    QString fileName = fileInfo.fileName();
    qint64 fileSize = file.size();

    // 检查是否为 Range 请求（断点续传）
    qint64 startPos = 0;
    qint64 endPos = fileSize - 1; // 默认读取整个文件
    bool isRangeRequest = false;

    // 获取请求头
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
            isRangeRequest = true;
            startPos = match.captured(1).toLongLong();
            if (!match.captured(2).isEmpty()) {
                endPos = match.captured(2).toLongLong();
            }
            // 如果 endPos 大于文件大小，则修正
            if (endPos >= fileSize) {
                endPos = fileSize - 1;
            }

            // 检查范围是否有效
            if (startPos > endPos || startPos >= fileSize) {
                QJsonObject jsonObject;
                jsonObject["error"] = "Requested Range Not Satisfiable";
                QJsonDocument jsonDoc(jsonObject);
                QByteArray jsonResponse = jsonDoc.toJson();

                QHttpServerResponse response("application/json", jsonResponse, QHttpServerResponse::StatusCode::RequestRangeNotSatisfiable);
                response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
                return response;
            }
        }
    }

    // 设置文件指针位置
    file.seek(startPos);
    qint64 bytesToRead = endPos - startPos + 1;

    // 分块读取文件
    QByteArray fileData = file.read(bytesToRead);

    // 设置响应头
    QHttpServerResponse response(fileData, isRangeRequest ? QHttpServerResponse::StatusCode::PartialContent : QHttpServerResponse::StatusCode::Ok);
    if (isRangeRequest) {
        // 206 Partial Content 响应
        response.setHeader("Content-Range", QString("bytes %1-%2/%3").arg(startPos, endPos, fileSize).toUtf8());
    }

    response.setHeader("Content-Type", "application/octet-stream");
    response.setHeader("Content-Length", QString::number(fileData.size()).toUtf8());
    response.setHeader("Content-Disposition", QString("attachment; filename=\"%1\"").arg(fileName).toUtf8());
    response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域

    return response;
}
