#include "UpdateChapterContentHandler.h"

#include <QHttpServerRequest>
#include <RogalunaHttpConfig.h>
#include <RogalunaLibraryServer.h>

#include <Macro/TokenGV.h>
#include <Macro/RequestBodyParser.h>

namespace Library {

QHttpServerResponse UpdateChapterContentHandler::handleRequest(const QHttpServerRequest &request)
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

    // 获取请求头中的数据
    QString bookId;
    QString chapterIndex;
    QString chapterContent;

    // 获取请求体（FormData）中的数据
    QByteArray bodyParts = request.body();

    PARSE_MULTIPART_FORM_DATA(bodyParts, contentType, formParts);

    // 从解析的 formParts 中提取必要的字段
    for (const MultipartPart &part : formParts) {
        if (part.name == "id") {
            bookId = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "index") {
            chapterIndex = QString::fromUtf8(part.data).trimmed();
        } else if (part.name == "content") {
            chapterContent = QString::fromUtf8(part.data).trimmed();
        }
    }

    // 检查是否收到了必须的参数
    if (bookId.isEmpty() || chapterIndex.isEmpty()) {
        QHttpServerResponse response("Missing metadata", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");
        return response;
    }

    // ================================================= 使用正则表达式的解决办法 ================= 弃用
    // 获取旧的内容
    // 从旧内容中提取资源引用 id
    // 从内容中提取出资源引用 id
    // 比对引用 id ，获取新增和移除的 id
    // 将新增的 id 文件复制持久存储，将移除的 id 文件复制到临时存储
    // 如果失败了，就不需要考虑其他的了，如果成功了，说明其他的文件操作都会成功，唯一的例外是磁盘空间不足，考虑这个，将文件回滚，即删除复制到持久存储和临时存储中的文件。
    // 如果成功了，那么更新章节内容
    // 内容更新失败，删除复制的文件
    // 内容更新成功，那么将资源 id 注册到数据库
    // 注册数据库失败，删除复制的文件，回滚之前的章节内容
    // 注册成功，将复制的文件的原件删除，完成任务。
    // ==============================================================================================

    // ======================= Myers 为核心的解决办法 ================================================= 弃用
    // 备份原始文件，执行内容更新操作
    // Myers 差异比较获取要进行增加、删除或修改的行，每一行进行正则匹配，获取资源引用
    // 计算应用后的数据库中的引用计数，获取会增加的行和会删除的行，得到需要移动的文件
    // 将需要移动的文件依次复制到持久或临时目录
    // 操作数据库，将资源引用计数增加或减少
    // 将备份的原始文件全部删除
    // 在不到最终操作之前出错造成终止，都需要还原旧的文件
    // ==============================================================================================

    // ======================= 传入修改值和全文的解决办法 =============================================== 弃用
    // 备份原始文件，执行内容更新操作
    // 计算应用后的数据库中的引用计数，获取会增加的行和会删除的行，得到需要移动的文件
    // 将需要移动的文件依次复制到持久或临时目录
    // 操作数据库，将资源引用计数增加或减少
    // 将备份的原始文件全部删除
    // 在不到最终操作之前出错造成终止，都需要还原旧的文件

    // ======================= 章节内使用的资源存放在章节内的解决办法
    // 获取原章节的全部内容，分离获取引用的资源
    // 传入新章节的全部内容，分离获取引用的资源
    // 得到差异资源，进行删除
    // 更新章节内容

    QByteArray oldChapterContent;
    qint64 s, e, size;
    QString title;
    bool bFetchOldContent = RogalunaHttpConfig::getInstance().getLibraryServer()->getChapterFile(
        bookId,
        chapterIndex.toInt(),
        false,
        oldChapterContent,
        s, e, size,
        title);

    if (!bFetchOldContent) {
        // 旧内容获取失败
        QString message = "Can't fetch old content!";
        QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    QStringList removedSrc = RogalunaHttpConfig::getInstance().getLibraryServer()->findRemovedResources(oldChapterContent, chapterContent);

    if (!RogalunaHttpConfig::getInstance().getLibraryServer()->updateChapterContent(bookId, chapterIndex, chapterContent)) {
        // 内容更新失败
        QHttpServerResponse response("Update content failed!", QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    // 删除不需要的资源
    static const QRegularExpression regex("token=([^&]*)");
    for (const QString &src : removedSrc) {
        QRegularExpressionMatch match = regex.match(src);
        if (match.hasMatch()) {
            // 提取匹配到的内容 (捕获组1)
            QString resExtractToken = match.captured(1);

            auto [isResValid, resHeader, resPayload] = VERIFY_JWT_TOKEN(resExtractToken, *RogalunaHttpConfig::getInstance().getSecretKey());
            if (!isResValid) {
                QHttpServerResponse response("Invalid res token", QHttpServerResponse::StatusCode::Unauthorized);
                response.setHeader("Access-Control-Allow-Origin", "*"); // 允许跨域
                return response;
            }

            QJsonDocument jwtResDoc = QJsonDocument::fromJson(resPayload.toUtf8());
            QJsonObject jwtResObj = jwtResDoc.object();

            QString chapterName = jwtResObj.value("name").toString();
            QString md5 = jwtResObj.value("md5").toString();

            if (!RogalunaHttpConfig::getInstance().getLibraryServer()->deleteChapterResource(bookId, chapterName, md5)) {
                qWarning() << "Delete file fail!";
            }

        } else {
            qWarning() << "No token found!";
        }
    }

    // 返回 JSON 响应
    QJsonObject jsonResponse;
    jsonResponse["success"] = true;

    QJsonDocument jsonDoc(jsonResponse);
    QByteArray jsonResponseData = jsonDoc.toJson();

    // 返回带有 CORS 头的 JSON 响应
    QHttpServerResponse response("application/json", jsonResponseData);
    response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
    return response;
}

}
