#include "MetadataDAO.h"

QString MetadataDAO::insertMetadata(int userId, const QString &fileName, const QString &contentMd5, bool isDirectory, const QString &parentUid, int version)
{
    QSqlQuery insertQuery(database);
    QString sql = QString("INSERT INTO %1 (user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at) "
                          "VALUES (:user_id, :file_name, :content_md5, :parent_uid, :is_directory, :version, NOW(), NOW())")
                      .arg(fullTableName());
    insertQuery.prepare(sql);
    insertQuery.bindValue(":user_id", userId);
    insertQuery.bindValue(":file_name", fileName);
    insertQuery.bindValue(":content_md5", contentMd5);
    insertQuery.bindValue(":parent_uid", parentUid.isEmpty() ? QString() : parentUid);
    insertQuery.bindValue(":is_directory", isDirectory);
    insertQuery.bindValue(":version", version);

    // 执行插入并获取新生成的 uid
    if (insertQuery.exec() && insertQuery.next()) {
        return insertQuery.value("uid").toString();  // 返回数据库生成的 uid
    }

    // 插入失败，记录错误并返回空字符串
    qWarning() << "Failed to insert new root directory for userId:" << userId << insertQuery.lastError().text();

    return QString();
}

bool MetadataDAO::deleteMetadata(const QString &uid)
{
    QSqlQuery query = createSchemaQuery(QString("DELETE FROM %1 WHERE uid = :uid").arg(fullTableName()));
    query.bindValue(":uid", uid);
    return executeQuery(query);
}

QString MetadataDAO::getUserRootDirUid(int userId)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT uid FROM %1 WHERE user_id = :user_id AND parent_uid IS NULL AND is_directory = TRUE").arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":user_id", userId);

    // 执行查询
    if (query.exec() && query.next()) {
        // 如果找到记录，则返回该记录的 uid
        return query.value("uid").toString();
    }

    // 如果没有找到根目录，插入一个新的根目录条目（数据库自动生成 uid）
    QString insertSql = QString("INSERT INTO %1 (user_id, file_name, parent_uid, is_directory, version, created_at, updated_at) "
                                "VALUES (:user_id, :file_name, NULL, TRUE, 1, NOW(), NOW()) RETURNING uid").arg(fullTableName());

    QSqlQuery insertQuery(database);
    insertQuery.prepare(insertSql);
    insertQuery.bindValue(":user_id", userId);
    insertQuery.bindValue(":file_name", "root"); // 设置根目录名称为 "root"

    // 执行插入并获取新生成的 uid
    if (insertQuery.exec() && insertQuery.next()) {
        return insertQuery.value("uid").toString();  // 返回数据库生成的 uid
    }

    // 插入失败，记录错误并返回空字符串
    qWarning() << "Failed to insert new root directory for userId:" << userId << insertQuery.lastError().text();
    return QString();
}

QString MetadataDAO::insertFolder(int userId, const QString &parentUid, const QString &folderName)
{
    QSqlQuery insertQuery(database);
    QString sql = QString("INSERT INTO %1 (user_id, file_name, parent_uid, is_directory, version) "
                                "VALUES (:user_id, :file_name, :parent_uid, TRUE, 1) RETURNING uid").arg(fullTableName());

    insertQuery.prepare(sql);
    insertQuery.bindValue(":user_id", userId);
    insertQuery.bindValue(":file_name", folderName); // 设置根目录名称为 folderName
    insertQuery.bindValue(":parent_uid", parentUid);

    // 执行插入并获取新生成的 uid
    if (insertQuery.exec() && insertQuery.next()) {
        return insertQuery.value("uid").toString();  // 返回数据库生成的 uid
    }

    // 插入失败，记录错误并返回空字符串
    qWarning() << "Failed to insert new root directory for userId:" << userId << insertQuery.lastError().text();
    return QString();
}

std::optional<QVector<FileMetadata>> MetadataDAO::getUserFiles(int userId)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT uid, user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at "
                          "FROM %1 WHERE user_id = :user_id").arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        return std::nullopt;
    }

    QVector<FileMetadata> files;

    // 循环遍历查询结果集
    while (query.next()) {
        FileMetadata metadata;
        metadata.uid = query.value("uid").toString();
        metadata.userId = query.value("user_id").toInt();
        metadata.fileName = query.value("file_name").toString();
        metadata.contentMd5 = query.value("content_md5").toString();
        metadata.parentUid = query.value("parent_uid").toString();
        metadata.isDirectory = query.value("is_directory").toBool();
        metadata.version = query.value("version").toInt();
        metadata.createdAt = query.value("created_at").toDateTime();
        metadata.updatedAt = query.value("updated_at").toDateTime();

        // 将每个文件的元数据加入到 QVector 中
        files.append(metadata);
    }

    // 如果没有查询到任何文件，返回 std::nullopt
    if (files.isEmpty()) {
        return std::nullopt;
    }

    return files;
}

std::optional<QVector<FileMetadata>> MetadataDAO::getUidFile(const QString &uid)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT uid, user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at "
                          "FROM %1 WHERE uid = :uid").arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":uid", uid);

    if (!query.exec()) {
        return std::nullopt;
    }

    QVector<FileMetadata> files;

    // 循环遍历查询结果集
    while (query.next()) {
        FileMetadata metadata;
        metadata.uid = query.value("uid").toString();
        metadata.userId = query.value("user_id").toInt();
        metadata.fileName = query.value("file_name").toString();
        metadata.contentMd5 = query.value("content_md5").toString();
        metadata.parentUid = query.value("parent_uid").toString();
        metadata.isDirectory = query.value("is_directory").toBool();
        metadata.version = query.value("version").toInt();
        metadata.createdAt = query.value("created_at").toDateTime();
        metadata.updatedAt = query.value("updated_at").toDateTime();

        // 将每个文件的元数据加入到 QVector 中
        files.append(metadata);
    }

    // 如果没有查询到任何文件，返回 std::nullopt
    if (files.isEmpty()) {
        return std::nullopt;
    }

    return files;
}

std::optional<QVector<FileMetadata>> MetadataDAO::getFolderFiles(const QString &folderUid)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT uid, user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at "
                          "FROM %1 WHERE parent_uid = :parent_uid").arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":parent_uid", folderUid);

    if (!query.exec()) {
        return std::nullopt;
    }

    QVector<FileMetadata> files;

    // 循环遍历查询结果集
    while (query.next()) {
        FileMetadata metadata;
        metadata.uid = query.value("uid").toString();
        metadata.userId = query.value("user_id").toInt();
        metadata.fileName = query.value("file_name").toString();
        metadata.contentMd5 = query.value("content_md5").toString();
        metadata.parentUid = query.value("parent_uid").toString();
        metadata.isDirectory = query.value("is_directory").toBool();
        metadata.version = query.value("version").toInt();
        metadata.createdAt = query.value("created_at").toDateTime();
        metadata.updatedAt = query.value("updated_at").toDateTime();

        // 将每个文件的元数据加入到 QVector 中
        files.append(metadata);
    }

    // 如果没有查询到任何文件，返回 std::nullopt
    if (files.isEmpty()) {
        return std::nullopt;
    }

    return files;
}
