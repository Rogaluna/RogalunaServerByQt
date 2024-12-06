#include "GetMusicHandler.h"

#include <QFile>
#include <QHttpServerRequest>
#include <QJsonArray>
#include <RogalunaHttpConfig.h>
#include <RogalunaMusicServer.h>

#include <Macro/TokenGV.h>

namespace MusicStation {

QHttpServerResponse GetMusicHandler::handleRequest(const QHttpServerRequest &request)
{
    // 遍历头部列表，查找 "Authorization" 头
    QList<QPair<QByteArray, QByteArray>> headers = request.headers();
    // QByteArray authorizationValue;
    // for (const auto &header : headers) {
    //     if (header.first.toLower() == "authorization") {
    //         authorizationValue = header.second;
    //         break;
    //     }
    // }

    // auto [isValid, header, payload] = VERIFY_JWT_TOKEN(QString(authorizationValue), *RogalunaHttpConfig::getInstance().getSecretKey());
    // // 令牌验证非法
    // if (!isValid) {
    //     QHttpServerResponse response("Invalid token", QHttpServerResponse::StatusCode::Unauthorized);
    //     response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
    //     return response;
    // }

    // // 令牌验证通过，获取负载对象
    // QJsonDocument jwtDoc = QJsonDocument::fromJson(payload.toUtf8());
    // QJsonObject jwtObj = jwtDoc.object();

    // // 验证时间
    // int checkResult = CHECK_TIMESTAMPS(jwtObj);
    // switch (checkResult) {
    // case 0:
    // {
    //     QHttpServerResponse response("Token Error: missing 'iat' or 'exp' claim.", QHttpServerResponse::StatusCode::Unauthorized);
    //     response.setHeader("Access-Control-Allow-Origin", "*");
    //     return response;
    // }
    // case 1:
    // {
    //     QHttpServerResponse response("Token Error: iat is in the future.", QHttpServerResponse::StatusCode::Unauthorized);
    //     response.setHeader("Access-Control-Allow-Origin", "*");
    //     return response;
    // }
    // case 2:
    // {
    //     QHttpServerResponse response("Token has expired!", QHttpServerResponse::StatusCode::Unauthorized);
    //     response.setHeader("Access-Control-Allow-Origin", "*");
    //     return response;
    // }
    // case 3:
    //     // 验证通过
    //     break;
    // }

    // // 获取负载数据中包含的用户信息
    // QString userId = jwtObj.value("id").toString();
    // 验证 userId 是否有权播放 （待实现）

    QUrlQuery query(request.url());
    QString musicId = "";

    // 获取 musicId 参数
    if (query.hasQueryItem("id")) {
        musicId = QUrl::fromPercentEncoding(query.queryItemValue("id").toUtf8());
    }

    // 根据 musicId 获取音乐元数据
    QString musicPath = "";
    QString musicName = "";
    QString fileType = "";
    QJsonArray results = RogalunaHttpConfig::getInstance().getMusicServer()->getMusicMetadata({musicId});
    if (results.first().isObject()) {
        QJsonObject result = results.first().toObject();
        musicPath = result.value("content_md5").toString();
        musicName = result.value("music_name").toString();
        fileType = result.value("file_type").toString();
    }

    if (musicPath.isEmpty()) {
        QHttpServerResponse response("No search target file.", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // 打开文件
    QFile musicFile(RogalunaHttpConfig::getInstance().getMusicServer()->getMusicFilePath(musicPath));
    if (!musicFile.exists() || !musicFile.open(QIODevice::ReadOnly)) {
        QJsonObject jsonObject;
        jsonObject["error"] = "File not found or cannot be opened";
        QJsonDocument jsonDoc(jsonObject);
        QByteArray jsonResponse = jsonDoc.toJson();

        QHttpServerResponse response("application/json", jsonResponse, QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // 文件大小
    qint64 fileSize = musicFile.size();

    // 检查 Range 请求
    qint64 startPos = 0;
    qint64 endPos = fileSize - 1;
    bool isRangeRequest = false;
    QString rangeHeader;
    for (const auto &header : request.headers()) {
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

            // 确保 endPos 不超过文件大小
            if (endPos >= fileSize) {
                endPos = fileSize - 1;
            }

            // 检查请求范围是否有效
            if (startPos > endPos || startPos >= fileSize) {
                QJsonObject jsonObject;
                jsonObject["error"] = "Requested Range Not Satisfiable";
                QJsonDocument jsonDoc(jsonObject);
                QByteArray jsonResponse = jsonDoc.toJson();

                QHttpServerResponse response("application/json", jsonResponse, QHttpServerResponse::StatusCode::RequestRangeNotSatisfiable);
                response.setHeader("Access-Control-Allow-Origin", "*");
                return response;
            }
        }
    }

    // 计算要读取的字节数
    qint64 bytesToRead = endPos - startPos + 1;

    // 限制每次传输的大小为 1MB
    qint64 maxBytesToRead = 1024 * 1024;  // 1MB
    if (bytesToRead > maxBytesToRead) {
        bytesToRead = maxBytesToRead;
        endPos = startPos + bytesToRead - 1;
    }

    // 设置文件指针位置
    musicFile.seek(startPos);

    // 读取文件数据
    QByteArray fileData = musicFile.read(bytesToRead);

    // 构建响应
    QHttpServerResponse response(fileData, isRangeRequest ? QHttpServerResponse::StatusCode::PartialContent : QHttpServerResponse::StatusCode::Ok);

    // 如果是 Range 请求，设置 Content-Range 响应头
    if (isRangeRequest) {
        response.setHeader("Content-Range", QString("bytes %1-%2/%3")
            .arg(QString::number(startPos), QString::number(endPos), QString::number(fileSize)).toUtf8());
    }

    // 设置 Content-Length
    response.setHeader("Content-Length", QString::number(fileData.size()).toUtf8());
    response.setHeader("Content-Type", "audio/*");

    // 设置其他必要的头部信息
    response.setHeader("Content-Disposition", "inline");
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}

}
