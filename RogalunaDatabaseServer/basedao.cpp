#include "basedao.h"

bool BaseDAO::executeQuery(QSqlQuery &query)
{
    if (!query.exec()) {
        qDebug() << "Query execution error:" << query.lastError();
        return false;
    }
    return true;
}

QSqlQuery BaseDAO::createSchemaQuery(const QString &sql)
{
    QSqlQuery query(database);
    query.prepare(sql);
    return query;
}

QString BaseDAO::fullTableName(const QString &tableName) const
{
    return QString("%1.%2").arg(schema, tableName);
}
