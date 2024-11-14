#include "CategoriesDAO.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace Library {

QJsonObject CategoriesDAO::getSubcategories(int parentId)
{
    QJsonArray allCategories = getAllCategories();  // 获取所有分类数据

    // 使用 QHash 存储分类对象和父子关系
    QHash<int, QJsonObject> categoryMap;
    QHash<int, QList<int>> childrenMap;

    // 初始化分类对象，并建立父子关系映射
    for (const QJsonValue& value : allCategories) {
        QJsonObject category = value.toObject();
        int id = category["id"].toInt();
        int parent = category["parent_id"].toInt();

        categoryMap[id] = category;  // 将分类存入 categoryMap

        // 如果有父节点，添加到 childrenMap 的父子关系中
        if (parent != -1) {
            childrenMap[parent].append(id);
        }
    }

    // 如果传入的 parentId 存在，则构建并返回对应的子树
    return categoryMap.contains(parentId) ? buildTree(parentId, childrenMap, categoryMap) : QJsonObject();
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

QMap<int, QString> CategoriesDAO::getCategoriesMapping()
{
    QMap<int, QString> mapping;
    QJsonArray allCategories = getAllCategories();

    for (const QJsonValue &value : allCategories) {
        QJsonObject category = value.toObject();
        int id = category["id"].toInt();
        QString name = category["name"].toString();
        mapping[id] = name;
    }

    return mapping;
}

QMap<QString, int> CategoriesDAO::getCategoriesInversionMapping()
{
    QMap<QString, int> inversionMapping;
    QJsonArray allCategories = getAllCategories();

    for (const QJsonValue &value : allCategories) {
        QJsonObject category = value.toObject();
        QString name = category["name"].toString();
        int id = category["id"].toInt();
        inversionMapping[name] = id;
    }

    return inversionMapping;
}

QJsonObject CategoriesDAO::buildTree(int id, const QHash<int, QList<int> > &childrenMap, QHash<int, QJsonObject> &categoryMap)
{
    QJsonObject node = categoryMap[id];  // 获取当前节点
    QJsonArray childrenArray;

    // 遍历所有子节点ID，并递归构建子树
    if (childrenMap.contains(id)) {
        for (int childId : childrenMap[id]) {
            childrenArray.append(buildTree(childId, childrenMap, categoryMap));  // 递归构建子节点
        }
    }

    node["children"] = childrenArray;  // 设置子节点数组
    return node;
}

QJsonArray CategoriesDAO::getAllCategories()
{
    QJsonArray categories;

    QString queryStr = QString("SELECT id, name, parent_id FROM %1").arg(fullTableName());
    QSqlQuery query = createSchemaQuery(queryStr);

    if (!query.exec()) {
        qWarning() << "获取分类数据失败:" << query.lastError().text();
        return categories;
    }

    // 将查询结果存入 QJsonArray
    while (query.next()) {
        QJsonObject category;
        category["id"] = query.value("id").toInt();
        category["name"] = query.value("name").toString();
        category["parent_id"] = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
        categories.append(category);
    }

    return categories;
}

}
