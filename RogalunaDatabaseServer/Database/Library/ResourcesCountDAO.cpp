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

QPair<bool, QStringList> Library::ResourcesCountDAO::addResourceCount(const QStringList &added)
{
    QStringList newIds;

    if (added.isEmpty()) {
        return qMakePair(true, newIds); // 空列表时直接返回成功
    }

    // 查询现有的 ID
    QString selectQueryStr = QString(
                                 "SELECT id FROM %1 WHERE id IN (%2)"
                                 ).arg(fullTableName());

    QStringList ids;
    for (const QString &resId : added) {
        ids.append(QString("'%1'").arg(resId));
    }

    selectQueryStr = selectQueryStr.arg(ids.join(", "));

    QSqlQuery selectQuery = createSchemaQuery(selectQueryStr);
    if (!executeQuery(selectQuery)) {
        return qMakePair(false, newIds);
    }

    // 记录已存在的 ID
    QSet<QString> existingIds;
    while (selectQuery.next()) {
        existingIds.insert(selectQuery.value("id").toString());
    }

    // 构建批量插入或更新的 SQL 查询
    QString queryStr = QString(
                           "INSERT INTO %1 (id, count) "
                           "VALUES %2 "
                           "ON CONFLICT (id) DO UPDATE SET count = count + 1"
                           ).arg(fullTableName());

    QStringList values;
    for (const QString &resId : added) {
        values.append(QString("('%1', 1)").arg(resId));
        if (!existingIds.contains(resId)) {
            newIds.append(resId); // 如果是新插入的 ID，记录下来
        }
    }

    queryStr = queryStr.arg(values.join(", "));

    QSqlQuery query = createSchemaQuery(queryStr);
    if (!executeQuery(query)) {
        return qMakePair(false, QStringList()); // 如果执行失败，返回 false 和空列表
    }

    return qMakePair(true, newIds); // 返回成功和新增的 ID 列表
}

QPair<bool, QStringList> Library::ResourcesCountDAO::removeResourceCount(const QStringList &removed)
{
    QStringList removedIds;

    if (removed.isEmpty()) {
        return qMakePair(true, removedIds); // 空列表时直接返回成功
    }

    // 更新 `count` 的 SQL 查询
    QString updateQueryStr = QString(
                                 "UPDATE %1 "
                                 "SET count = count - 1 "
                                 "WHERE id IN (%2) AND count > 0"
                                 ).arg(fullTableName());

    QStringList ids;
    for (const QString &resId : removed) {
        ids.append(QString("'%1'").arg(resId));
    }

    updateQueryStr = updateQueryStr.arg(ids.join(", "));

    QSqlQuery updateQuery = createSchemaQuery(updateQueryStr);
    if (!executeQuery(updateQuery)) {
        return qMakePair(false, QStringList()); // 如果更新失败，返回 false 和空列表
    }

    // 查询完全移除的 ID
    QString selectQueryStr = QString(
                                 "SELECT id FROM %1 "
                                 "WHERE count <= 0 AND id IN (%2)"
                                 ).arg(fullTableName());

    selectQueryStr = selectQueryStr.arg(ids.join(", "));

    QSqlQuery selectQuery = createSchemaQuery(selectQueryStr);
    if (!executeQuery(selectQuery)) {
        return qMakePair(false, QStringList()); // 如果查询失败，返回 false 和空列表
    }

    while (selectQuery.next()) {
        removedIds.append(selectQuery.value("id").toString());
    }

    // 删除完全移除的记录
    QString deleteQueryStr = QString(
                                 "DELETE FROM %1 "
                                 "WHERE id IN (%2) AND count <= 0"
                                 ).arg(fullTableName());

    deleteQueryStr = deleteQueryStr.arg(ids.join(", "));

    QSqlQuery deleteQuery = createSchemaQuery(deleteQueryStr);
    if (!executeQuery(deleteQuery)) {
        return qMakePair(false, QStringList()); // 如果删除失败，返回 false 和空列表
    }

    return qMakePair(true, removedIds); // 返回成功和完全移除的 ID 列表
}
