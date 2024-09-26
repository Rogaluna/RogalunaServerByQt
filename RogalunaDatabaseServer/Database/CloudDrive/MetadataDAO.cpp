#include "MetadataDAO.h"

#include <QDir>

namespace CloudDrive {

QString MetadataDAO::insertMetadata(int userId, const QString &fileName, const QString &contentMd5, bool isDirectory, const QString &parentUid, int version)
{
    QSqlQuery insertQuery(database);
    QString sql = QString("INSERT INTO %1 (user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at) "
                          "VALUES (:user_id, :file_name, :content_md5, :parent_uid, :is_directory, :version, NOW(), NOW()) "
                          "RETURNING uid")
                      .arg(fullTableName());
    insertQuery.prepare(sql);
    insertQuery.bindValue(":user_id", userId);
    insertQuery.bindValue(":file_name", fileName);
    insertQuery.bindValue(":content_md5", contentMd5);
    insertQuery.bindValue(":parent_uid", parentUid.isEmpty() ? QString() : parentUid);
    insertQuery.bindValue(":is_directory", isDirectory);
    insertQuery.bindValue(":version", version);

    // 执行插入并获取新生成的 uid
    if (executeQuery(insertQuery) && insertQuery.next()) {
        return insertQuery.value(0).toString();  // 返回数据库生成的 uid
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
    if (executeQuery(query) && query.next()) {
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
    if (executeQuery(insertQuery) && insertQuery.next()) {
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
    if (executeQuery(insertQuery) && insertQuery.next()) {
        return insertQuery.value("uid").toString();  // 返回数据库生成的 uid
    }

    // 插入失败，记录错误并返回空字符串
    qWarning() << "Failed to insert new root directory for userId:" << userId << insertQuery.lastError().text();
    return QString();
}

QString MetadataDAO::getPath(const QString &uid)
{
    QSqlQuery query(database);
    QString path;
    QString currentUid = uid;

    // 循环获取每个父目录的文件名，构建完整的展平路径
    while (!currentUid.isEmpty()) {
        QString sql = QString("SELECT file_name, parent_uid FROM %1 WHERE uid = :uid").arg(fullTableName());
        query.prepare(sql);
        query.bindValue(":uid", currentUid);

        if (executeQuery(query) && query.next()) {
            QString fileName = query.value("file_name").toString();
            QString parentUid = query.value("parent_uid").toString();

            // 构建路径：如果路径不为空，则追加分隔符
            if (!path.isEmpty()) {
                path = QDir::separator() + path;
            }
            path = fileName + path;

            // 继续处理父级目录
            currentUid = parentUid;
        } else {
            qWarning() << "Failed to retrieve path for UID:" << uid << query.lastError().text();
            return QString();
        }
    }

    return path;
}

std::optional<FFileMetadata> MetadataDAO::getMetadataFromPath(const QString &path, int userId)
{
    // 将路径按 '/' 分割为各级目录或文件
    QStringList pathParts = path.split(QDir::separator(), Qt::SkipEmptyParts);

    // 初始化当前的 UID 为用户的根目录 UID
    QString currentUid = getUserRootDirUid(userId); // 获取用户根目录 UID
    if (currentUid.isEmpty()) {
        qWarning() << "Failed to retrieve root directory for userId:" << userId;
        return std::nullopt;
    }

    // 遍历路径的每一部分，从根向下逐级查找
    for (const QString& part : pathParts) {
        QSqlQuery query(database);
        QString sql = QString("SELECT uid, file_name, is_directory, parent_uid, version, content_md5 "
                              "FROM %1 WHERE file_name = :file_name AND parent_uid = :parent_uid AND user_id = :user_id")
                          .arg(fullTableName());

        query.prepare(sql);
        query.bindValue(":file_name", part);
        query.bindValue(":parent_uid", currentUid);  // 使用当前 UID 作为父级 UID
        query.bindValue(":user_id", userId);  // 限定用户 ID

        // 执行查询并获取当前部分的元数据
        if (executeQuery(query) && query.next()) {
            currentUid = query.value("uid").toString();  // 更新当前 UID 为查询到的 UID
        } else {
            qWarning() << "Failed to retrieve metadata for path part:" << part << "with userId:" << userId << query.lastError().text();
            return std::nullopt; // 如果某一级查询失败，返回 std::nullopt
        }
    }

    // 在成功找到完整路径后，构建并返回最终的 FileMetadata 对象
    QSqlQuery finalQuery(database);
    QString finalSql = QString("SELECT uid, file_name, is_directory, parent_uid, version, content_md5 "
                               "FROM %1 WHERE uid = :uid AND user_id = :user_id")
                           .arg(fullTableName());

    finalQuery.prepare(finalSql);
    finalQuery.bindValue(":uid", currentUid);
    finalQuery.bindValue(":user_id", userId);

    // 执行最终查询以获取完整的元数据
    if (executeQuery(finalQuery) && finalQuery.next()) {
        FFileMetadata metadata;
        metadata.uid = finalQuery.value("uid").toString();
        metadata.fileName = finalQuery.value("file_name").toString();
        metadata.isDirectory = finalQuery.value("is_directory").toBool();
        metadata.parentUid = finalQuery.value("parent_uid").toString();
        metadata.version = finalQuery.value("version").toInt();
        metadata.contentMd5 = finalQuery.value("content_md5").toString();

        return metadata;  // 返回元数据
    }

    // 如果查询失败，返回 std::nullopt
    qWarning() << "Failed to retrieve final metadata for UID:" << currentUid << "with userId:" << userId << finalQuery.lastError().text();
    return std::nullopt;
}

std::optional<QVector<FFileMetadata>> MetadataDAO::getMetadataByUserId(int userId)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT uid, user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at "
                          "FROM %1 WHERE user_id = :user_id").arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":user_id", userId);

    if (!executeQuery(query)) {
        return std::nullopt;
    }

    QVector<FFileMetadata> files;

    // 循环遍历查询结果集
    while (query.next()) {
        FFileMetadata metadata;
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

std::optional<QVector<FFileMetadata>> MetadataDAO::getMetadataByUid(const QString &uid)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT uid, user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at "
                          "FROM %1 WHERE uid = :uid").arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":uid", uid);

    if (!executeQuery(query)) {
        return std::nullopt;
    }

    QVector<FFileMetadata> files;

    // 循环遍历查询结果集
    while (query.next()) {
        FFileMetadata metadata;
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

std::optional<QVector<FFileMetadata> > MetadataDAO::getMetadataUnderFolder(const QString &folderUid)
{
    QSqlQuery query(database);
    QString sql = QString("SELECT uid, user_id, file_name, content_md5, parent_uid, is_directory, version, created_at, updated_at "
                          "FROM %1 WHERE parent_uid = :parent_uid").arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":parent_uid", folderUid);

    if (!executeQuery(query)) {
        return std::nullopt;
    }

    QVector<FFileMetadata> files;

    // 循环遍历查询结果集
    while (query.next()) {
        FFileMetadata metadata;
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

bool MetadataDAO::updateFileName(const QString &uid, const QString &name)
{
    QSqlQuery query(database);
    QString sql = QString("UPDATE %1 SET file_name = :file_name, updated_at = :updated_at WHERE uid = :uid")
                      .arg(fullTableName());

    query.prepare(sql);
    query.bindValue(":file_name", name);
    query.bindValue(":updated_at", QDateTime::currentDateTime());
    query.bindValue(":uid", uid);

    // 执行更新操作，如果失败返回 false
    if (!executeQuery(query)) {
        return false;
    }

    // 检查受影响的行数，如果没有更新任何行，表示更新失败
    if (query.numRowsAffected() == 0) {
        return false;
    }

    return true;
}

int MetadataDAO::getRefCount(const QString &field, const QString &value)
{
    QSqlQuery query(database);

    // 使用占位符构建 SQL 查询
    QString sql = QString("SELECT COUNT(*) FROM %1 WHERE %2 = :value").arg(fullTableName(), field);

    query.prepare(sql);
    query.bindValue(":value", value);

    // 执行查询，如果失败返回 -1 表示出错
    if (!query.exec()) {
        return -1;
    }

    // 解析查询结果
    if (query.next()) {
        return query.value(0).toInt(); // 返回计数值
    }

    // 如果查询结果为空，返回 0
    return 0;
}


}
