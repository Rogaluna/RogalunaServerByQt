#include "UserCollectionsDAO.h"

QStringList Library::UserCollectionsDAO::getBooksByUserCollection(const QString &userId)
{
    QStringList bookIds;

    // 查询字符串，获取指定用户收藏的书籍 ID
    QString queryStr = QString(
                           "SELECT book_id "
                           "FROM %1 "
                           "WHERE user_id = :userId"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":userId", userId);

    // 执行查询
    if (!query.exec()) {
        qWarning() << "Failed to retrieve user collections:" << query.lastError();
        return bookIds;  // 查询失败，返回空的 QStringList
    }

    // 遍历查询结果，将每个 book_id 添加到 QStringList 中
    while (query.next()) {
        bookIds.append(query.value("book_id").toString());
    }

    // 返回包含用户收藏书籍 ID 的 QStringList
    return bookIds;
}
