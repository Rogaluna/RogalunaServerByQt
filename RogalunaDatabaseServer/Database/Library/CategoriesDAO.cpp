#include "CategoriesDAO.h"

QMap<int, QString> CategoriesDAO::getAllCategories()
{
    QMap<int, QString> categories;
    QString queryStr = QString("SELECT id, name FROM %1").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);

    if (executeQuery(query)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            categories.insert(id, name);
        }
    } else {
        qWarning() << "Failed to retrieve categories:" << query.lastError().text();
    }

    return categories;
}

QList<QPair<int, QString> > CategoriesDAO::getSubcategories(int parentId)
{
    QList<QPair<int, QString>> subcategories;
    QString queryStr = QString("SELECT id, name FROM %1 WHERE parent_id = :parentId").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":parentId", parentId);

    if (executeQuery(query)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            subcategories.append(qMakePair(id, name));
        }
    } else {
        qWarning() << "Failed to retrieve subcategories:" << query.lastError().text();
    }

    return subcategories;
}

bool CategoriesDAO::addCategory(const QString &name, int parentId)
{
    QString queryStr = QString("INSERT INTO %1 (name, parent_id) VALUES (:name, :parentId)").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":name", name);
    query.bindValue(":parentId", parentId);

    return executeQuery(query);
}

bool CategoriesDAO::deleteCategory(int categoryId)
{
    QString queryStr = QString("DELETE FROM %1 WHERE id = :categoryId").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);
    query.bindValue(":categoryId", categoryId);

    return executeQuery(query);
}
