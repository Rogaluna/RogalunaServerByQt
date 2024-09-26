#include "MetadataDAO.h"

#include <QJsonArray>
#include <QJsonObject>

namespace MusicStation {

QString MusicStation::MetadataDAO::insertMetadata(
    int userId,
    const QString &fileType,
    const QString &musicName,
    int duration,
    const QString &artist,
    const QString &contentMd5,
    const QString &description,
    const QString &albumId,
    int bitrate,
    bool bPublished)
{
    QSqlQuery insertQuery(database);
    QString sql = QString("INSERT INTO %1 (user_id, file_type, music_name, duration, artist, content_md5, description, album_id, bitrate, is_published) "
                          "VALUES (:user_id, :file_type, :music_name, :duration, :artist, :content_md5, :description, :album_id, :bitrate, :is_published) RETURNING uid")
                      .arg(fullTableName());
    insertQuery.prepare(sql);
    insertQuery.bindValue(":user_id", userId);
    insertQuery.bindValue(":file_type", fileType);
    insertQuery.bindValue(":music_name", musicName);
    insertQuery.bindValue(":duration", duration);
    insertQuery.bindValue(":artist", artist);
    insertQuery.bindValue(":content_md5", contentMd5);
    insertQuery.bindValue(":description", description);
    insertQuery.bindValue(":album_id", albumId);
    insertQuery.bindValue(":bitrate", bitrate);
    insertQuery.bindValue(":is_published", bPublished);

    // 执行插入并获取新生成的 uid
    if (insertQuery.exec() && insertQuery.next()) {
        return insertQuery.value("uid").toString();  // 返回数据库生成的 uid
    }

    // 插入失败，记录错误并返回空字符串
    qWarning() << "Failed to insert new music for userId:" << userId << insertQuery.lastError().text();

    return QString();
}

QJsonArray MetadataDAO::getMetadataByUid(const QStringList &uids)
{
    if (uids.isEmpty()) {
        return QJsonArray();  // 如果 uid 列表为空，直接返回空数组
    }

    QSqlQuery query(database);
    // 构建占位符字符串
    QStringList quotedUids;
    for (const QString &uid : uids) {
        quotedUids << QString("'%1'").arg(uid);  // 给每个 uid 加上单引号
    }
    QString placeholders = quotedUids.join(", ");  // 用逗号连接带引号的 uid
    QString sql = QString("SELECT * FROM %1 WHERE uid IN (%2)").arg(fullTableName(), placeholders);
    query.prepare(sql);
    // 绑定每个 uid 到占位符
    for (int i = 0; i < uids.size(); ++i) {
        query.bindValue(i, uids.at(i));
    }

    if (!query.exec()) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return QJsonArray();  // 查询失败，返回空数组
    }

    QJsonArray metadataArray;
    if (query.next()) {
        QJsonObject metadata;
        metadata["uid"] = query.value("uid").toString();
        metadata["user_id"] = query.value("user_id").toInt();
        metadata["file_type"] = query.value("file_type").toString();
        metadata["music_name"] = query.value("music_name").toString();
        metadata["duration"] = query.value("duration").toInt();
        metadata["content_md5"] = query.value("content_md5").toString();
        metadata["description"] = query.value("description").toString();
        metadata["album_id"] = query.value("album_id").toString();
        metadata["bitrate"] = query.value("bitrate").toInt();
        metadata["is_published"] = query.value("is_published").toBool();

        metadataArray.append(metadata);  // 将 JSON 对象添加到数组中
    }

    return metadataArray;
}

QJsonArray MetadataDAO::getRandomMetadata()
{
    QSqlQuery query(database);
    QString sql = QString("SELECT * FROM %1 ORDER BY RANDOM() LIMIT 10").arg(fullTableName());
    query.prepare(sql);

    if (!query.exec()) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return QJsonArray();  // 查询失败，返回空数组
    }

    QJsonArray metadataArray;
    while (query.next()) {
        QJsonObject metadata;
        metadata["uid"] = query.value("uid").toString();
        metadata["user_id"] = query.value("user_id").toInt();
        metadata["file_type"] = query.value("file_type").toString();
        metadata["music_name"] = query.value("music_name").toString();
        metadata["duration"] = query.value("duration").toInt();
        metadata["artist"] = query.value("artist").toString();
        metadata["content_md5"] = query.value("content_md5").toString();
        metadata["description"] = query.value("description").toString();
        metadata["album_id"] = query.value("album_id").toString();
        metadata["bitrate"] = query.value("bitrate").toInt();
        metadata["is_published"] = query.value("is_published").toBool();

        metadataArray.append(metadata);  // 将 JSON 对象添加到数组中
    }

    return metadataArray;  // 返回包含所有元数据的数组
}

QJsonArray MetadataDAO::getMetadataByUserId(int userId)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT * FROM %1 WHERE user_id = :userId").arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return QJsonArray();  // 查询失败，返回空数组
    }

    QJsonArray metadataArray;
    while (query.next()) {
        QJsonObject metadata;
        metadata["uid"] = query.value("uid").toString();
        metadata["user_id"] = query.value("user_id").toInt();
        metadata["file_type"] = query.value("file_type").toString();
        metadata["music_name"] = query.value("music_name").toString();
        metadata["duration"] = query.value("duration").toInt();
        metadata["artist"] = query.value("artist").toString();
        metadata["content_md5"] = query.value("content_md5").toString();
        metadata["description"] = query.value("description").toString();
        metadata["album_id"] = query.value("album_id").toString();
        metadata["bitrate"] = query.value("bitrate").toInt();
        metadata["is_published"] = query.value("is_published").toBool();

        metadataArray.append(metadata);
    }

    return metadataArray;
}

QJsonArray MetadataDAO::getMetadataByAlbumId(const QString &albumId)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT * FROM %1 WHERE album_id = :albumId").arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":albumId", albumId);

    if (!query.exec()) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return QJsonArray();  // 查询失败，返回空数组
    }

    QJsonArray metadataArray;
    while (query.next()) {
        QJsonObject metadata;
        metadata["uid"] = query.value("uid").toString();
        metadata["user_id"] = query.value("user_id").toInt();
        metadata["file_type"] = query.value("file_type").toString();
        metadata["music_name"] = query.value("music_name").toString();
        metadata["duration"] = query.value("duration").toInt();
        metadata["artist"] = query.value("artist").toString();
        metadata["content_md5"] = query.value("content_md5").toString();
        metadata["description"] = query.value("description").toString();
        metadata["album_id"] = query.value("album_id").toString();
        metadata["bitrate"] = query.value("bitrate").toInt();
        metadata["is_published"] = query.value("is_published").toBool();

        metadataArray.append(metadata);
    }

    return metadataArray;
}

QJsonArray MetadataDAO::getMetadataByUserIdAndPublished(int userId, bool bPublished)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT * FROM %1 WHERE user_id = :userId AND is_published = :isPublished").arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":userId", userId);
    query.bindValue(":isPublished", bPublished);

    if (!query.exec()) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return QJsonArray();  // 查询失败，返回空数组
    }

    QJsonArray metadataArray;
    while (query.next()) {
        QJsonObject metadata;
        metadata["uid"] = query.value("uid").toString();
        metadata["user_id"] = query.value("user_id").toInt();
        metadata["file_type"] = query.value("file_type").toString();
        metadata["music_name"] = query.value("music_name").toString();
        metadata["duration"] = query.value("duration").toInt();
        metadata["artist"] = query.value("artist").toString();
        metadata["content_md5"] = query.value("content_md5").toString();
        metadata["description"] = query.value("description").toString();
        metadata["album_id"] = query.value("album_id").toString();
        metadata["bitrate"] = query.value("bitrate").toInt();
        metadata["is_published"] = query.value("is_published").toBool();

        metadataArray.append(metadata);
    }

    return metadataArray;
}

}
