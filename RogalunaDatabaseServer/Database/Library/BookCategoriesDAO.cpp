#include "BookCategoriesDAO.h"

#include <QJsonArray>

bool Library::BookCategoriesDAO::addBookTag(const QString &bookId, int tagId)
{
    QString queryStr = QString("INSERT INTO %1 (book_id, category_id) VALUES (:bookId, :tagId)")
    .arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);
    query.bindValue(":tagId", tagId);

    return executeQuery(query);
}

bool Library::BookCategoriesDAO::addBookTags(const QString &bookId, const QVector<int> &tagIds)
{
    if (tagIds.isEmpty()) return true;  // 如果没有标签ID，直接返回成功

    // 构建批量插入SQL语句
    QString queryStr = QString("INSERT INTO %1 (book_id, category_id) VALUES ").arg(fullTableName());

    QStringList valueStrings;
    for (int i = 0; i < tagIds.size(); ++i) {
        valueStrings << QString("(:bookId, :tagId%1)").arg(i);  // 占位符
    }
    queryStr += valueStrings.join(", ");  // 将所有值拼接成SQL语句

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);

    // 绑定所有标签ID到对应的占位符
    for (int i = 0; i < tagIds.size(); ++i) {
        query.bindValue(QString(":tagId%1").arg(i), tagIds[i]);
    }

    return executeQuery(query);
}

bool Library::BookCategoriesDAO::deleteBookTag(const QString &bookId, int tagId)
{
    QString queryStr = QString("DELETE FROM %1 WHERE book_id = :bookId AND category_id = :tagId")
    .arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);
    query.bindValue(":tagId", tagId);

    return executeQuery(query);
}

bool Library::BookCategoriesDAO::deleteAllBookTags(const QString &bookId)
{
    QString queryStr = QString("DELETE FROM %1 WHERE book_id = :bookId").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);

    return executeQuery(query);
}

QJsonObject Library::BookCategoriesDAO::getBookTags(const QString &bookId)
{
    // 构建查询语句
    QString queryStr = QString("SELECT category_id FROM %1 WHERE book_id = :bookId")
                           .arg(fullTableName());

    // 创建查询对象
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);

    // 执行查询
    if (!executeQuery(query)) {
        return {};  // 查询失败，返回空的 QJsonObject
    }

    // 构建返回的 QJsonObject
    QJsonObject result;
    QJsonArray tagArray;
    while (query.next()) {
        int tagId = query.value(0).toInt();
        tagArray.append(tagId);
    }

    // 添加结果到 QJsonObject 中
    result["id"] = bookId;
    result["tags"] = tagArray;

    return result;
}
