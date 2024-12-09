#include "ContentDAO.h"

bool CloudDrive::ContentDAO::insertContent(const QString &contentMd5, qint64 size)
{
    QSqlQuery query(database);
    QString sql = QString("INSERT INTO %1 (content_md5, size, created_at) "
                          "VALUES (:content_md5, :size, NOW())")
                      .arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":content_md5", contentMd5);
    query.bindValue(":size", size);
    return executeQuery(query);
}

bool CloudDrive::ContentDAO::deleteContent(const QString &contentMd5)
{
    QSqlQuery query = createSchemaQuery(QString("DELETE FROM %1 WHERE content_md5 = :content_md5").arg(fullTableName()));
    query.bindValue(":content_md5", contentMd5);
    return executeQuery(query);
}
