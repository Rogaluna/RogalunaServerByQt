#include "Database/testdao.h"

bool TestDAO::createTable() {
    QSqlQuery query(database);
    query.prepare("CREATE TABLE IF NOT EXISTS test (id SERIAL PRIMARY KEY, name VARCHAR(255))");
    return executeQuery(query);
}

bool TestDAO::insertTest(const QString& name) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO test (name) VALUES (:name)");
    query.bindValue(":name", name);
    return executeQuery(query);
}

QList<QPair<int, QString>> TestDAO::getAllTests() {
    QList<QPair<int, QString>> results;
    QSqlQuery query(database);
    query.prepare("SELECT * FROM test");
    if (executeQuery(query)) {
        while (query.next()) {
            int id = query.value("id").toInt();
            QString name = query.value("name").toString();
            results.append(QPair<int, QString>(id, name));
        }
    }
    return results;
}

QPair<int, QString> TestDAO::getTestById(int id) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM test WHERE id = :id");
    query.bindValue(":id", id);
    if (executeQuery(query) && query.next()) {
        return QPair<int, QString>(query.value("id").toInt(), query.value("name").toString());
    }
    return QPair<int, QString>(-1, QString());
}
