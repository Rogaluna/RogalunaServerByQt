#include "BooksDAO.h"

QJsonArray Library::BooksDAO::getBooksByRandom(int rangeSize)
{
    QJsonArray booksArray;

    QString queryStr = QString(
                           "SELECT id, user_id, name, description "
                           "FROM %1 "
                           "ORDER BY RANDOM() "
                           "LIMIT :rangeSize"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":rangeSize", rangeSize);

    // 执行查询
    if (!query.exec()) {
        qWarning() << "Failed to retrieve random books:" << query.lastError();
        return booksArray;  // 查询失败，返回空的 QJsonArray
    }

    // 遍历查询结果并构建 JSON 数组
    while (query.next()) {
        QJsonObject bookObject;
        bookObject["id"] = query.value("id").toString();
        bookObject["user_id"] = query.value("user_id").toString();
        bookObject["name"] = query.value("name").toString();
        bookObject["description"] = query.value("description").toString();

        booksArray.append(bookObject);
    }

    // 返回包含随机书籍信息的 QJsonArray
    return booksArray;
}

QJsonArray Library::BooksDAO::getBooksByUserWrite(const QString &userId, int rangeSize)
{
    QJsonArray booksArray;

    // 定义查询字符串，根据 user_id 筛选书籍并限制返回数量
    QString queryStr = QString(
                           "SELECT id, name, description "
                           "FROM %1 "
                           "WHERE user_id = :userId "
                           "LIMIT :rangeSize"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":userId", userId);
    query.bindValue(":rangeSize", rangeSize);

    // 执行查询
    if (!query.exec()) {
        qWarning() << "Failed to retrieve books by user:" << query.lastError();
        return booksArray;  // 查询失败，返回空的 QJsonArray
    }

    // 遍历查询结果并构建 JSON 数组
    while (query.next()) {
        QJsonObject bookObject;
        bookObject["id"] = query.value("id").toString();
        bookObject["user_id"] = userId;
        bookObject["name"] = query.value("name").toString();
        bookObject["description"] = query.value("description").toString();

        booksArray.append(bookObject);
    }

    // 返回包含指定用户书籍信息的 QJsonArray
    return booksArray;
}

QJsonArray Library::BooksDAO::getBooksByIds(QStringList bookIds)
{
    QJsonArray booksArray;

    if (bookIds.isEmpty()) {
        return booksArray;  // 如果 bookIds 为空，直接返回空的 QJsonArray
    }

    // 将书籍ID列表转换为SQL的IN子句
    QString placeholders;
    for (int i = 0; i < bookIds.size(); ++i) {
        placeholders += (i > 0 ? ", " : "") + QString(":id%1").arg(i);
    }

    QString queryStr = QString(
                           "SELECT id, user_id, name, description "
                           "FROM %1 "
                           "WHERE id IN (%2)"
                           ).arg(fullTableName(), placeholders);

    QSqlQuery query = createSchemaQuery(queryStr);

    // 绑定每个书籍ID到查询的占位符
    for (int i = 0; i < bookIds.size(); ++i) {
        query.bindValue(QString(":id%1").arg(i), bookIds.at(i));
    }

    // 执行查询
    if (!query.exec()) {
        qWarning() << "Failed to retrieve books by IDs:" << query.lastError();
        return booksArray;  // 查询失败，返回空的 QJsonArray
    }

    // 遍历查询结果，将每条书籍记录转换为 JSON 对象并添加到 JSON 数组中
    while (query.next()) {
        QJsonObject bookObject;
        bookObject["id"] = query.value("id").toString();
        bookObject["user_id"] = query.value("user_id").toString();
        bookObject["name"] = query.value("name").toString();
        bookObject["description"] = query.value("description").toString();

        booksArray.append(bookObject);
    }

    // 返回包含书籍信息的 QJsonArray
    return booksArray;
}

QJsonObject Library::BooksDAO::getBookDetails(const QString &bookId)
{
    // 查询书籍详细信息
    QString queryStr = QString(
                           "SELECT id, name, description, user_id, created_at "
                           "FROM %1 "
                           "WHERE id = :bookId"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);

    // 执行查询并检查结果
    if (!query.exec() || !query.next()) {
        qWarning() << "Failed to fetch book details:" << query.lastError();
        return {};  // 如果失败，返回空的 QJsonObject
    }

    // 创建 JSON 对象并填充书籍信息
    QJsonObject bookDetails;
    bookDetails["id"] = query.value("id").toString();
    bookDetails["name"] = query.value("name").toString();
    bookDetails["description"] = query.value("description").toString();
    bookDetails["user_id"] = query.value("user_id").toString();
    bookDetails["created_at"] = query.value("created_at").toString();

    // 返回书籍详细信息的 JSON 对象
    return bookDetails;
}

QString Library::BooksDAO::addBook(const QString &userId, const QString &bookName, const QString &bookDesc)
{
    // 插入书籍信息并返回生成的 book_id (UUID)
    QString queryStr = QString(
                           "INSERT INTO %1 (user_id, name, description) "
                           "VALUES (:userId, :bookName, :bookDesc) "
                           "RETURNING id"
                           ).arg(fullTableName());

    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":userId", userId);
    query.bindValue(":bookName", bookName);
    query.bindValue(":bookDesc", bookDesc);

    // 执行查询并获取生成的 book_id
    if (!query.exec() || !query.next()) {
        qWarning() << "Failed to insert book:" << query.lastError();
        return "";  // 如果失败，返回空字符串
    }

    // 返回生成的书籍 UUID
    return query.value(0).toString();
}

bool Library::BooksDAO::updateBook(const QString &bookId, const QString &bookName, const QString &bookDesc)
{
    // 构建更新语句
    QString queryStr = QString(
                           "UPDATE %1 "
                           "SET name = :bookName, description = :bookDesc "
                           "WHERE id = :bookId"
                           ).arg(fullTableName());

    // 创建查询对象
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookName", bookName);
    query.bindValue(":bookDesc", bookDesc);
    query.bindValue(":bookId", bookId);

    // 执行查询并检查结果
    if (!query.exec()) {
        qWarning() << "Failed to update book:" << query.lastError();
        return false;
    }

    // 检查受影响的行数
    if (query.numRowsAffected() == 0) {
        qWarning() << "No book found with id:" << bookId;
        return false;
    }

    return true; // 更新成功
}

bool Library::BooksDAO::deleteBook(const QString &bookId)
{
    // 构建删除语句
    QString queryStr = QString(
                           "DELETE FROM %1 "
                           "WHERE id = :bookId"
                           ).arg(fullTableName());

    // 创建查询对象
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":bookId", bookId);

    // 执行查询并检查结果
    if (!query.exec()) {
        qWarning() << "Failed to delete book:" << query.lastError();
        return false;
    }

    // 检查受影响的行数
    if (query.numRowsAffected() == 0) {
        qWarning() << "No book found with id:" << bookId;
        return false;
    }

    return true; // 删除成功
}
