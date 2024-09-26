#include "UsersDAO.h"

namespace Account {

UserDAO::UserDAO(QSqlDatabase& db, const QString& schema, const QString& tableName): BaseDAO(db, schema, tableName) {}

QJsonObject UserDAO::registerUser(const QString& username, const QString& passwordHash,
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

    QJsonObject result;

    if (executeQuery(query) && query.next()) {
        // 获取新插入的用户ID
        int userId = query.value("id").toInt();
        // 如果 authority 没有提供，默认为 "normal"
        QString authValue = authority ? *authority : "normal";

        result["userId"] = userId;
        result["username"] = username;
        result["authority"] = authValue;
    }

    return result;  // 返回插入结果
}

bool UserDAO::deleteUser(int userId)
{
    QString sql = QString("DELETE FROM %1 WHERE id = :id").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(sql);
    query.bindValue(":id", userId);
    return query.exec();
}

QJsonObject UserDAO::updateUser(int userId,
                                        const std::optional<QString>& username,
                                        const std::optional<QString>& passwordHash,
                                        const std::optional<QString>& authority)
{
    // 动态构建 SQL 语句
    QString sql = QString("UPDATE %1 SET ").arg(fullTableName());
    QStringList setClauses;
    QJsonObject result;

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
        return result;  // 如果没有任何字段需要更新，返回空值
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
        // 写入更新后的用户信息
        result["userId"] = query.value("id").toInt();
        result["username"] = query.value("username").toString();
        result["authority"] = query.value("authority").toString();
    }

    return result;  // 返回结果值
}

QJsonObject UserDAO::getUserById(int userId)
{
    QJsonObject result;

    QString sql = QString("SELECT id, username, password, authority FROM %1 WHERE id = :id").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(sql);
    query.bindValue(":id", userId);

    if (executeQuery(query) && query.next()) {
        result["userId"] = query.value("id").toInt();
        result["username"] = query.value("username").toString();
        result["password"] = query.value("password").toString();
        result["authority"] = query.value("authority").toString();
    }
    return result;
}

QJsonObject UserDAO::getUserByName(QString username)
{
    QJsonObject result;

    QString sql = QString("SELECT id, username, password, authority FROM %1 WHERE username = :username").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(sql);
    query.bindValue(":username", username);

    if (executeQuery(query) && query.next()) {
        result["userId"] = query.value("id").toInt();
        result["username"] = query.value("username").toString();
        result["password"] = query.value("password").toString();
        result["authority"] = query.value("authority").toString();
    }
    return result;  // 未找到
}

}
