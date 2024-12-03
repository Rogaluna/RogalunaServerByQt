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
        return query.value("ref_count").toInt();
    }

    return 0;
}

bool Library::ResourcesCountDAO::insertResourceCount(const QString &resId, int count)
{
    // 构建插入查询语句，使用占位符防止SQL注入
    QString queryStr = QString(
                           "INSERT INTO %1 (id, ref_count) "
                           "VALUES (:resId, :count)"
                           ).arg(fullTableName());

    // 创建 QSqlQuery 对象
    QSqlQuery query = createSchemaQuery(queryStr);

    // 绑定参数
    query.bindValue(":resId", resId);
    query.bindValue(":count", count);

    // 执行查询并检查是否成功
    if (executeQuery(query)) {
        return true;
    }

    return false;
}

bool Library::ResourcesCountDAO::deleteResourceCount(const QString &resId)
{
    // 构建删除查询语句，使用占位符防止SQL注入
    QString queryStr = QString(
                           "DELETE FROM %1 "
                           "WHERE id = :resId"
                           ).arg(fullTableName());

    // 创建 QSqlQuery 对象
    QSqlQuery query = createSchemaQuery(queryStr);

    // 绑定参数
    query.bindValue(":resId", resId);

    // 执行查询并检查是否成功
    if (executeQuery(query)) {
        if (query.numRowsAffected() > 0) {
            return true;
        }
    }

    return false;
}

bool Library::ResourcesCountDAO::modifyResourceCount(const QString &resId, int count)
{
    // 构建更新查询语句，使用占位符来防止SQL注入
    QString queryStr = QString(
                           "UPDATE %1 "
                           "SET ref_count = :count "
                           "WHERE id = :resId"
                           ).arg(fullTableName());

    // 创建 QSqlQuery 对象
    QSqlQuery query = createSchemaQuery(queryStr);

    // 绑定参数，确保类型匹配
    query.bindValue(":resId", resId);
    query.bindValue(":count", count);

    // 执行查询并检查是否成功
    if (executeQuery(query)) {
        // 检查是否有行被更新，确保 resId 存在
        if (query.numRowsAffected() > 0) {
            return true;
        }
    }

    return false;
}

