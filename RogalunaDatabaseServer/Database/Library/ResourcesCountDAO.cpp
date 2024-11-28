#include "ResourcesCountDAO.h"

int Library::ResourcesCountDAO::getResourceCount(const QString &resId)
{
    QString queryStr = QString(
                           "SELECT ref_count "
                           "FROM %1 "
                           "WHERE id = :resId"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":resId", resId);

    if (executeQuery(query) && query.next()) {
        return query.value("count").toInt();
    }

    return 0;
}

QString Library::ResourcesCountDAO::getResourceType(const QString &resId)
{
    QString queryStr = QString(
                           "SELECT file_type "
                           "FROM %1 "
                           "WHERE id = :resId"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":resId", resId);

    if (executeQuery(query) && query.next()) {
        return query.value("file_type").toString();
    }

    return 0;
}
