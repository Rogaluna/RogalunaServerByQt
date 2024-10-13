#include "MetadataDAO.h"

namespace MusicStation {

QString MusicStation::MetadataDAO::insertMetadata(
    int userId,
    const QString &fileName,
    const QString &contentMd5,
    const QString &description,
    const QString &albumId,
    int bitrate,
    bool bPublished)
{
    QSqlQuery insertQuery(database);
    QString sql = QString("INSERT INTO %1 (user_id, file_name, content_md5, description, album_id, bitrate, is_published) "
                          "VALUES (:user_id, :file_name, :content_md5, :description, :album_id, :bitrate, :is_published)")
                      .arg(fullTableName());
    insertQuery.prepare(sql);
    insertQuery.bindValue(":user_id", userId);
    insertQuery.bindValue(":file_name", fileName);
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

std::optional<FFileMetadata> MetadataDAO::getMetadataByUid(const QString &uid)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT * FROM %1 WHERE uid = :uid").arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":uid", uid);

    if (!query.exec()) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        FFileMetadata metadata;
        metadata.uid = query.value("uid").toString();
        metadata.userId = query.value("user_id").toInt();
        metadata.fileName = query.value("file_name").toString();
        metadata.contentMd5 = query.value("content_md5").toString();
        metadata.description = query.value("description").toString();
        metadata.albumId = query.value("album_id").toInt();
        metadata.bitrate = query.value("bitrate").toInt();
        metadata.bPublished = query.value("is_published").toBool();
        // 根据您的实际字段，继续填充 metadata 对象

        return metadata;
    } else {
        // 未找到记录
        return std::nullopt;
    }
}

std::optional<QVector<FFileMetadata> > MetadataDAO::getRandomMetadata()
{
    QSqlQuery query(database);
    QString sql = QString("SELECT * FROM %1 ORDER BY RANDOM() LIMIT 10").arg(fullTableName()); // 获取随机10条记录
    if (!query.exec(sql)) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return std::nullopt;
    }

    QVector<FFileMetadata> metadataList;
    while (query.next()) {
        FFileMetadata metadata;
        metadata.uid = query.value("uid").toString();
        metadata.userId = query.value("user_id").toInt();
        metadata.fileName = query.value("file_name").toString();
        metadata.contentMd5 = query.value("content_md5").toString();
        metadata.description = query.value("description").toString();
        metadata.albumId = query.value("album_id").toInt();
        metadata.bitrate = query.value("bitrate").toInt();
        metadata.bPublished = query.value("is_published").toBool();
        // 根据您的实际字段，继续填充 metadata 对象

        metadataList.append(metadata);
    }

    if (!metadataList.isEmpty()) {
        return metadataList;
    } else {
        // 未找到任何记录
        return std::nullopt;
    }
}

}
