#ifndef CATEGORIESDAO_H
#define CATEGORIESDAO_H

#include <BaseDAO.h>
#include <QJsonObject>
#include <Interface/Library/ICategoriesDAO.h>

namespace Library {

class ROGALUNADATABASESERVER_EXPORT CategoriesDAO : public BaseDAO, public ICategoriesDAO
{
public:
    explicit CategoriesDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "categories") {}

    // 实现 ICategoriesDAO 的接口

    QJsonObject getSubcategories(int parentId = 0) override;
    bool addCategory(const QString& name, int parentId) override;
    bool deleteCategory(int categoryId) override;

    QMap<int, QString> getCategoriesMapping();  // ID -> 名称映射
    QMap<QString, int> getCategoriesInversionMapping();  // 名称 -> ID 映射

private:
    QJsonObject buildTree(int id, const QHash<int, QList<int>>& childrenMap, QHash<int, QJsonObject>& categoryMap);  // 递归构建子树
    QJsonArray getAllCategories();  // 获取所有分类数据
};

}

#endif // CATEGORIESDAO_H
