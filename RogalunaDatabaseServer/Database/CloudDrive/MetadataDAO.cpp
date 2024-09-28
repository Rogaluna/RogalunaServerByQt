#include "MetadataDAO.h"

bool MetadataDAO::insertMetadata(const QString &uid, int userId, const QString &fileName, bool isDirectory, const QString &parentUid, int version)
{
    QSqlQuery query(database);
    QString sql = QString("INSERT INTO %1 (uid, user_id, file_name, parent_uid, is_directory, version, uploaded_at, created_at) "
                          "VALUES (:uid, :user_id, :file_name, :parent_uid, :is_directory, :version, NOW(), NOW())")
                      .arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":uid", uid);
    query.bindValue(":user_id", userId);
    query.bindValue(":file_name", fileName);
    query.bindValue(":parent_uid", parentUid.isEmpty() ? QVariant() : parentUid);
    query.bindValue(":is_directory", isDirectory);
    query.bindValue(":version", version);
    return executeQuery(query);
}

std::optional<FileMetadata> MetadataDAO::getUserFile(const QString &uid, int userId)
{
    QSqlQuery query(database);
    query.prepare("SELECT uid, user_id, file_name, is_directory, parent_uid, version, uploaded_at "
                  "FROM file_metadata WHERE uid = :uid AND user_id = :user_id");
    query.bindValue(":uid", uid);
    query.bindValue(":user_id", userId);

    if (!query.exec() || !query.next()) {
        return std::nullopt;  // 没有找到文件或查询失败
    }

    FileMetadata metadata;
    metadata.uid = query.value("uid").toString();
    metadata.userId = query.value("user_id").toInt();
    metadata.fileName = query.value("file_name").toString();
    metadata.isDirectory = query.value("is_directory").toBool();
    metadata.parentUid = query.value("parent_uid").toString();
    metadata.version = query.value("version").toInt();
    metadata.uploadedAt = query.value("uploaded_at").toDateTime();

    return metadata;
}

bool MetadataDAO::deleteMetadata(const QString &uid)
{
    QSqlQuery query = createSchemaQuery(QString("DELETE FROM %1 WHERE uid = :uid").arg(fullTableName()));
    query.bindValue(":uid", uid);
    return executeQuery(query);
}
