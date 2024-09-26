#include "GetMusicListHandler.h"

#include <QHttpServerRequest>
#include <QJsonArray>
#include <RogalunaHttpConfig.h>
#include <RogalunaMusicServer.h>

#include <Macro/RequestBodyParser.h>
#include <Macro/TokenGV.h>

namespace MusicStation {

QHttpServerResponse GetMusicListHandler::handleRequest(const QHttpServerRequest &request)
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

    QUrlQuery query(request.url());
    RogalunaMusicServer::EMusicQueryType opt = RogalunaMusicServer::EMusicQueryType::E_RANDOM;
    QString param = "";

    if (query.hasQueryItem("opt")) {
        // 如果查询参数 "opt" 存在
        int numOpt = query.queryItemValue("opt").toInt();


        // 检查 numOpt 是否在枚举的有效范围内
        if (numOpt >= static_cast<int>(RogalunaMusicServer::EMusicQueryType::E_RANDOM) &&
            numOpt <= static_cast<int>(RogalunaMusicServer::EMusicQueryType::E_OTHERS)) {

            // 将 numOpt 转换为 EMusicQueryType 并赋值给 opt
            opt = static_cast<RogalunaMusicServer::EMusicQueryType>(numOpt);
        } else {
            // 处理无效的 opt 值
            opt = RogalunaMusicServer::EMusicQueryType::E_RANDOM; // 设为默认值
        }
    } else {
        // 如果没有 opt 参数，返回错误响应
        QHttpServerResponse response("Missing opt in query parameters", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    if (query.hasQueryItem("param")) {
        // 如果查询参数 "param" 存在
        param = query.queryItemValue("param");
        param = QUrl::fromPercentEncoding(param.toUtf8());

        if (param == "#") {
            // 如果 param 使用 # ，则替换为用户 id
            param = userId;
        }

    } else {
        // 如果没有 param 参数，返回错误响应
        QHttpServerResponse response("Missing param in query parameters", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    QJsonArray musicList = RogalunaHttpConfig::getInstance().getMusicServer()->getMusicList(opt, param);

    // 构建JSON响应
    // QJsonArray jsonArray;
    // for (const FileMetadata &entry : entries) {
    //     QJsonObject jsonObject;
    //     jsonObject["uid"] = entry.uid;
    //     jsonObject["name"] = entry.fileName;
    //     jsonObject["contentMd5"] = entry.contentMd5;
    //     jsonObject["lastModified"] = entry.updatedAt.toString(Qt::ISODate);
    //     jsonObject["type"] = (entry.isDirectory ? "dir" : (entry.fileName.lastIndexOf('.') != -1 ? entry.fileName.mid(entry.fileName.lastIndexOf('.') + 1) : ""));
    //     // jsonObject["size"] = entry.size;

    //     jsonArray.append(jsonObject);
    // }

    QJsonObject responseObject;
    responseObject["success"] = true;
    responseObject["data"] = musicList;
    QJsonDocument jsonDoc(responseObject);
    QByteArray jsonResponse = jsonDoc.toJson();

    // 返回带有 JSON 数据的 QHttpServerResponse
    QHttpServerResponse response("application/json", jsonResponse);
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}

}
