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

    QHash<QString, int> removedIds;
    QHash<QString, int> addedIds;

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
        } else if (part.name == "removed") {
            removedIds[QString::fromUtf8(part.data).trimmed()]++;
        } else if (part.name == "added") {
            addedIds[QString::fromUtf8(part.data).trimmed()]++;
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

    // ======================= 传入修改值和全文的解决办法
    // 备份原始文件，执行内容更新操作
    // 计算应用后的数据库中的引用计数，获取会增加的行和会删除的行，得到需要移动的文件
    // 将需要移动的文件依次复制到持久或临时目录
    // 操作数据库，将资源引用计数增加或减少
    // 将备份的原始文件全部删除
    // 在不到最终操作之前出错造成终止，都需要还原旧的文件

    QStringList deleteIds;
    QStringList insertIds;

    const QString &chapterFile = RogalunaHttpConfig::getInstance().getLibraryServer()->getChapterFilePath(bookId, chapterIndex);

    if (!QFile::copy(chapterFile, chapterFile + ".copy")) {
        // 备份原始章节失败
        QString message = "Backup chapter failed!";
        QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    if (!RogalunaHttpConfig::getInstance().getLibraryServer()->updateChapterContent(bookId, chapterIndex, chapterContent)) {
        // 内容更新失败
        QString message = "Update content failed!";
        // 删除备份章节
        if (!QFile::remove(chapterFile + ".copy")) {
            // 没有正确回滚，非常严重的错误
            qWarning() << "Rollback fail: backup chapter can't delete.";
        }

        QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
        response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
        return response;
    }

    for (const QString &key : removedIds.keys()) {
        // 获取键当前的资源引用计数
        int currentRefCount = RogalunaHttpConfig::getInstance().getLibraryServer()->queryResCount(key);
        if (currentRefCount - removedIds.value(key) <= 0) {
            // 引用计数归零，记入删除表
            deleteIds.append(key);
        }
    }

    for (const QString &key : addedIds.keys()) {
        int currentRefCount = RogalunaHttpConfig::getInstance().getLibraryServer()->queryResCount(key);
        if (currentRefCount == 0) {
            // 引用计数非零，记入插入表
            insertIds.append(key);
        }
    }

    // 将标记为删除的 id 文件进行复制
    for (const QString &id : deleteIds) {
        if (RogalunaHttpConfig::getInstance().getLibraryServer()->isResPersExist(id)) {
            // 持久目录下文件存在

            const QString & sourceTarget = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
                RogalunaHttpConfig::getInstance().getLibraryServer()->getResFolderPath() + QDir::separator() + id);

            QDir dir(sourceTarget);
            QStringList fileList = dir.entryList(QDir::Files);

            const QString &copyTarget = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
                id, RogalunaHttpConfig::getInstance().getStorageServer()->temp);
            QDir tempDir(copyTarget);

            if (!tempDir.exists()) {
                tempDir.mkpath("."); // 如果临时文件夹不存在，则创建
            } else {
                // 异常目录存在，必须回滚
                QString message = "Failed: Exist exception directory.";
                QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
                response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
                return response;
            }

            QFile::copy(sourceTarget + QDir::separator() + fileList.first(), copyTarget + QDir::separator() + fileList.first());
        } else {
            // 资源不存在，必须回滚

            // 替换更新的章节为备份的

            // 将已经复制的资源删除

            QString message = "Failed: The resource does not exist.";
            QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
            return response;
        }
    }

    for (const QString &id : insertIds) {
        if (RogalunaHttpConfig::getInstance().getLibraryServer()->isResTempExist(id)) {
            // 临时目录下文件存在

            const QString & sourceTarget = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
                id, RogalunaHttpConfig::getInstance().getStorageServer()->temp);

            QDir dir(sourceTarget);
            QStringList fileList = dir.entryList(QDir::Files);

            const QString &copyTarget = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
                RogalunaHttpConfig::getInstance().getLibraryServer()->getResFolderPath() + QDir::separator() + id);
            QDir tempDir(copyTarget);

            if (!tempDir.exists()) {
                tempDir.mkpath("."); // 如果临时文件夹不存在，则创建
            } else {
                // 异常目录存在，必须回滚
                QString message = "Failed: Exist exception directory.";
                QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
                response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
                return response;
            }

            QFile::copy(sourceTarget + QDir::separator() + fileList.first(), copyTarget + QDir::separator() + fileList.first());
        } else {
            // 资源不存在，必须回滚

            // 替换更新的章节为备份的

            // 将已经复制的资源删除

            QString message = "Failed: The resource does not exist.";
            QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
            return response;
        }
    }

    // 操作数据库，将资源引用计数增加或减少
    for (const QString &key : removedIds.keys()) {
        if (!RogalunaHttpConfig::getInstance().getLibraryServer()->modifyResCount(key, -removedIds.value(key))) {
            // 数据库操作失败，回滚

            // 替换更新的章节为备份的

            // 将已经复制的资源删除

            QString message = "Database operation failed!";
            QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
            return response;
        }
    }
    for (const QString &key : addedIds.keys()) {
        if (!RogalunaHttpConfig::getInstance().getLibraryServer()->modifyResCount(key, addedIds.value(key))) {
            // 数据库操作失败，回滚

            // 替换更新的章节为备份的

            // 将已经复制的资源删除

            QString message = "Database operation failed!";
            QHttpServerResponse response(message, QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin", "*");  // 允许跨域请求
            return response;
        }
    }

    // 将备份的原始文件全部删除
    QFile::remove(chapterFile + ".copy");

    for (const QString &id : deleteIds) {
        // 删除的资源，将备份的文件和原始文件都删除
        const QString & sourceTarget_1 = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
            id, RogalunaHttpConfig::getInstance().getStorageServer()->temp);

        QDir dir1(sourceTarget_1);
        dir1.removeRecursively();

        const QString & sourceTarget_2 = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
            RogalunaHttpConfig::getInstance().getLibraryServer()->getResFolderPath() + QDir::separator() + id);

        QDir dir2(sourceTarget_2);
        dir2.removeRecursively();
    }

    for (const QString &id : insertIds) {
        // 新增的资源，将备份的文件删除
        const QString & sourceTarget = RogalunaHttpConfig::getInstance().getStorageServer()->absoluteFilePath(
            id, RogalunaHttpConfig::getInstance().getStorageServer()->temp);

        QDir dir(sourceTarget);
        dir.removeRecursively();
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
