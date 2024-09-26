#ifndef CATEGORIESDAO_H
#define CATEGORIESDAO_H

#include <BaseDAO.h>
#include <Interface/Library/ICategoriesDAO.h>

class ROGALUNADATABASESERVER_EXPORT CategoriesDAO : public BaseDAO, public ICategoriesDAO
{
public:
    explicit CategoriesDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "categories") {}

    // 实现 ICategoriesDAO 的接口
    QMap<int, QString> getAllCategories() override;
    QList<QPair<int, QString>> getSubcategories(int parentId) override;
    bool addCategory(const QString& name, int parentId) override;
    bool deleteCategory(int categoryId) override;
};

#endif // CATEGORIESDAO_H
