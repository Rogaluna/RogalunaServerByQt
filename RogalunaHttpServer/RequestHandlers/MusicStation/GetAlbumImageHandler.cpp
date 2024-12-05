#include "GetAlbumImageHandler.h"

#include <QHttpServerRequest>
#include <RogalunaHttpConfig.h>
#include <RogalunaMusicServer.h>

#include <Macro/TokenGV.h>

namespace MusicStation {

QHttpServerResponse GetAlbumImageHandler::handleRequest(const QHttpServerRequest &request)
{
    QList<QPair<QByteArray, QByteArray>> headers = request.headers();

    // 遍历头部列表，查找 "Cookie" 头中的 "token" ，实际上它就是 "authorization"
    QByteArray authorizationValue;
    for (const QPair<QByteArray, QByteArray> &header : headers) {
        // if (header.first.toLower() == "authorization") {
        //     authorizationValue = header.second;
        //     break;
        // }
        if (header.first.toLower() == "cookie") {
            QStringList cookies = QString(header.second).split("; ");
            for (const QString &cookie : cookies) {
                if (cookie.startsWith("token=")) {
                    authorizationValue = cookie.section('=', 1).toUtf8();
                    break;
                }
            }
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
    // 提取资源 id
    QString id;
    if (query.hasQueryItem("id")) {
        id = query.queryItemValue("id");
    } else {
        // 如果没有 id 参数，返回错误响应
        QHttpServerResponse response("Missing query parameters", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
        return response;
    }

    // 获取文件
    QByteArray result = RogalunaHttpConfig::getInstance().getMusicServer()->getAlbumImage(id);

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response(result, QHttpServerResponse::StatusCode::Ok);
    response.setHeader("Content-Type", "image/*");
    response.setHeader("Content-Length", QString::number(result.size()).toUtf8());
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}

}
