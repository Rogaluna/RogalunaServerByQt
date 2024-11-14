#include "ReadProgressDAO.h"


bool Library::ReadProgressDAO::updateProgress(int userId, const QString &bookId, int chapterNumber, int progress)
{
    QString queryStr = QString(
                           "INSERT INTO %1 (user_id, book_id, chapter_number, progress, updated_at) "
                           "VALUES (:userId, :bookId, :chapterNumber, :progress, NOW()) "
                           "ON CONFLICT (user_id, book_id) "
                           "DO UPDATE SET chapter_number = :chapterNumber, progress = :progress, updated_at = NOW()"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    query.bindValue(":chapterNumber", chapterNumber);
    query.bindValue(":progress", progress);

    return executeQuery(query);
}

QJsonObject Library::ReadProgressDAO::getProgress(int userId, const QString &bookId)
{
    QJsonObject result;

    QString queryStr = QString(
                           "SELECT chapter_number, progress, updated_at "
                           "FROM %1 "
                           "WHERE user_id = :userId AND book_id = :bookId"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);

    if (executeQuery(query) && query.next()) {
        result["chapterNumber"] = query.value("chapter_number").toInt();
        result["progress"] = query.value("progress").toInt();
        result["updatedAt"] = query.value("updated_at").toString();
        return result;
    }

    return result; // 未找到则返回空的 QJsonObject
}

bool Library::ReadProgressDAO::deleteProgress(int userId, const QString &bookId)
{
    QString queryStr = QString(
                           "DELETE FROM %1 "
                           "WHERE user_id = :userId AND book_id = :bookId"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);

    return executeQuery(query);
}
