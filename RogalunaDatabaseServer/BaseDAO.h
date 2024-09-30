#ifndef BASEDAO_H
#define BASEDAO_H

#include "RogalunaDatabaseServer_Global.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

class ROGALUNADATABASESERVER_EXPORT BaseDAO
{
public:
    explicit BaseDAO(QSqlDatabase& db, const QString& schema = "public", const QString& tableName = "")
        : database(db), schema(schema), tableName(tableName) {}

    virtual ~BaseDAO() {}

protected:
    QSqlDatabase& database;
    QString schema;
    QString tableName;

    bool executeQuery(QSqlQuery& query);
    QSqlQuery createSchemaQuery(const QString& sql);

    // 获得表的包含Schema的全称
    QString fullTableName() const;
};

#endif // BASEDAO_H
