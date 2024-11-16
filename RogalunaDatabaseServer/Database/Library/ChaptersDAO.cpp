#include "ChaptersDAO.h"

#include <QJsonArray>

bool Library::ChaptersDAO::addChapter(const QString &bookId, int chapterNumber, const QString &title, const QString &group, const QString &fileName)
{
    QString queryStr = QString(
                           "INSERT INTO %1 (book_id, chapter_number, title, \"group\", file_name) "
                           "VALUES (:bookId, :chapterNumber, :title, :group, :fileName)"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);
    query.bindValue(":chapterNumber", chapterNumber);
    query.bindValue(":title", title);
    query.bindValue(":group", group);
    query.bindValue(":fileName", fileName);

    return executeQuery(query);
}

QJsonObject Library::ChaptersDAO::getChapterDetails(const QString &bookId, int chapterNumber)
{
    QString queryStr = QString(
                           "SELECT book_id, chapter_number, title, \"group\", word_count, file_name, created_at, updated_at "
                           "FROM %1 "
                           "WHERE book_id = :bookId AND chapter_number = :chapterNumber"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);
    query.bindValue(":chapterNumber", chapterNumber);

    if (!query.exec() || !query.next()) {
        qWarning() << "Failed to fetch chapter details:" << query.lastError();
        return {};
    }

    QJsonObject chapterDetails;
    chapterDetails["book_id"] = query.value("book_id").toString();
    chapterDetails["chapter_number"] = query.value("chapter_number").toInt();
    chapterDetails["title"] = query.value("title").toString();
    chapterDetails["group"] = query.value("group").toString();
    chapterDetails["word_count"] = query.value("word_count").toInt();
    chapterDetails["file_name"] = query.value("file_name").toString();
    chapterDetails["created_at"] = query.value("created_at").toString();
    chapterDetails["updated_at"] = query.value("updated_at").toString();

    return chapterDetails;
}

bool Library::ChaptersDAO::updateChapter(const QString &bookId, int chapterNumber, int newChapterNumber, const QString &title, const QString &group, int wordCount)
{
    QString queryStr = QString(
                           "UPDATE %1 "
                           "SET chapter_number = :newChapterNumber, title = :title, \"group\" = :group, word_count = :wordCount, updated_at = NOW() "
                           "WHERE book_id = :bookId AND chapter_number = :chapterNumber"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":newChapterNumber", newChapterNumber);
    query.bindValue(":title", title);
    query.bindValue(":group", group);
    query.bindValue(":wordCount", wordCount);
    query.bindValue(":bookId", bookId);
    query.bindValue(":chapterNumber", chapterNumber);

    return executeQuery(query);
}

bool Library::ChaptersDAO::deleteChapter(const QString &bookId, int chapterNumber)
{
    QString queryStr = QString(
                           "DELETE FROM %1 "
                           "WHERE book_id = :bookId AND chapter_number = :chapterNumber"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);
    query.bindValue(":chapterNumber", chapterNumber);

    return executeQuery(query);
}

QJsonArray Library::ChaptersDAO::listChapters(const QString &bookId)
{
    QString queryStr = QString(
                           "SELECT book_id, chapter_number, title, \"group\", word_count, created_at, updated_at "
                           "FROM %1 "
                           "WHERE book_id = :bookId "
                           "ORDER BY chapter_number"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);

    QJsonArray chaptersArray;
    if (query.exec()) {
        while (query.next()) {
            QJsonObject chapterDetails;
            chapterDetails["book_id"] = query.value("book_id").toString();
            chapterDetails["chapter_number"] = query.value("chapter_number").toInt();
            chapterDetails["title"] = query.value("title").toString();
            chapterDetails["group"] = query.value("group").toString();
            chapterDetails["word_count"] = query.value("word_count").toInt();
            chapterDetails["created_at"] = query.value("created_at").toString();
            chapterDetails["updated_at"] = query.value("updated_at").toString();
            chaptersArray.append(chapterDetails);
        }
    } else {
        qWarning() << "Failed to list chapters:" << query.lastError();
    }

    return chaptersArray;
}

QPair<QVector<QString>, bool> Library::ChaptersDAO::getAllChapterFileName(const QString &bookId)
{
    QString queryStr = QString(
                           "SELECT file_name "
                           "FROM %1 "
                           "WHERE book_id = :bookId "
                           "ORDER BY chapter_number"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);

    QVector<QString> fileNameArray;

    if (query.exec()) {
        while (query.next()) {
            fileNameArray.append(query.value("file_name").toString());
        }
        return qMakePair(fileNameArray, true);
    } else {
        qWarning() << "Failed to execute query:" << query.lastError();
        return qMakePair(fileNameArray, false);
    }
}
