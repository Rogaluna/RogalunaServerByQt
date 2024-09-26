#include "UsersDAO.h"

UserDAO::UserDAO(QSqlDatabase& db, const QString& schema, const QString& tableName): BaseDAO(db, schema, tableName) {}

std::optional<User> UserDAO::registerUser(const QString& username, const QString& passwordHash,
                                          const std::optional<QString>& authority)
{
    QString sql = QString("INSERT INTO %1 (username, password").arg(fullTableName());

    // 动态构建 SQL 语句
    if (authority) {
        sql += ", authority";
    }
    sql += ") VALUES (:username, :password";
    if (authority) {
        sql += ", :authority";
    }
    sql += ") RETURNING id";

    QSqlQuery query = createSchemaQuery(sql);
    query.bindValue(":username", username);
    query.bindValue(":password", passwordHash);

    if (authority) {
        query.addBindValue(*authority);
    }

    if (executeQuery(query) && query.next()) {
        // 获取新插入的用户ID
        int userId = query.value("id").toInt();
        // 如果 authority 没有提供，默认为 "normal"
        QString authValue = authority ? *authority : "normal";
        return User(userId, username, authValue);
    }

    return std::nullopt;  // 插入失败，返回空值
}

bool UserDAO::deleteUser(int userId)
{
    QString sql = QString("DELETE FROM %1 WHERE id = :id").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(sql);
    query.bindValue(":id", userId);
    return query.exec();
}

std::optional<User> UserDAO::updateUser(int userId,
                                        const std::optional<QString>& username,
                                        const std::optional<QString>& passwordHash,
                                        const std::optional<QString>& authority)
{
    // 动态构建 SQL 语句
    QString sql = QString("UPDATE %1 SET ").arg(fullTableName());
    QStringList setClauses;

    if (username) {
        setClauses << "username = :username";
    }
    if (passwordHash) {
        setClauses << "password = :password";
    }
    if (authority) {
        setClauses << "authority = :authority";
    }

    if (setClauses.isEmpty()) {
        return std::nullopt;  // 如果没有任何字段需要更新，返回空值
    }

    sql += setClauses.join(", ");
    sql += " WHERE id = :id RETURNING id, username, authority";

    QSqlQuery query = createSchemaQuery(sql);

    // 绑定提供的值
    if (username) {
        query.bindValue(":username", *username);
    }
    if (passwordHash) {
        query.bindValue(":password", *passwordHash);
    }
    if (authority) {
        query.bindValue(":authority", *authority);
    }
    query.bindValue(":id", userId);

    if (executeQuery(query) && query.next()) {
        // 返回更新后的用户信息
        return User(query.value("id").toInt(),
                    query.value("username").toString(),
                    query.value("authority").toString());
    }

    return std::nullopt;  // 更新失败，返回空值
}

std::optional<User> UserDAO::getUserById(int userId)
{
    QString sql = QString("SELECT id, username, password, authority FROM %1 WHERE id = :id").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(sql);
    query.bindValue(":id", userId);

    if (executeQuery(query) && query.next()) {
        User user(query.value("id").toInt(),
                  query.value("username").toString(),
                  query.value("password").toString(),
                  query.value("authority").toString());
        return user;  // 返回 User 对象的 std::optional
    }
    return std::nullopt;  // 未找到
}

std::optional<User> UserDAO::getUserByName(QString username)
{
    QString sql = QString("SELECT id, username, password, authority FROM %1 WHERE username = :username").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(sql);
    query.bindValue(":username", username);

    if (executeQuery(query) && query.next()) {
        User user(query.value("id").toInt(),
                  query.value("username").toString(),
                  query.value("password").toString(),
                  query.value("authority").toString());
        return user;  // 返回 User 对象的 std::optional
    }
    return std::nullopt;  // 未找到
}
