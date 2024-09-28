#include "ContentDAO.h"

bool ContentDAO::insertContent(const QString &contentMd5, qint64 size, const QString &fileType, const QString &path)
{
    QSqlQuery query(database);
    QString sql = QString("INSERT INTO %1 (content_md5, size, file_type, path, created_at) "
                          "VALUES (:content_md5, :size, :file_type, :path, NOW())")
                      .arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":content_md5", contentMd5);
    query.bindValue(":size", size);
    query.bindValue(":file_type", fileType);
    query.bindValue(":path", path);
    return executeQuery(query);
}

QString ContentDAO::getContentPath(const QString &contentMd5)
{
    QSqlQuery query = createSchemaQuery(QString("SELECT path FROM %1 WHERE content_md5 = :content_md5").arg(fullTableName()));
    query.bindValue(":content_md5", contentMd5);
    if (executeQuery(query) && query.next()) {
        return query.value("path").toString();
    }
    return "";
}

bool ContentDAO::deleteContent(const QString &contentMd5)
{
    QSqlQuery query = createSchemaQuery(QString("DELETE FROM %1 WHERE content_md5 = :content_md5").arg(fullTableName()));
    query.bindValue(":content_md5", contentMd5);
    return executeQuery(query);
}
