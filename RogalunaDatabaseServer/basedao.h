#ifndef BASEDAO_H
#define BASEDAO_H

#include "RogalunaDatabaseServer_global.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

class ROGALUNADATABASESERVER_EXPORT BaseDAO
{
public:
    explicit BaseDAO(QSqlDatabase& db, const QString& schema = "public")
        : database(db), schema(schema) {}

    virtual ~BaseDAO() {}

protected:
    QSqlDatabase& database;
    QString schema;

    bool executeQuery(QSqlQuery& query);
    QSqlQuery createSchemaQuery(const QString& sql);
    QString fullTableName(const QString& tableName) const;
};

#endif // BASEDAO_H
