#ifndef ICATEGORIESDAO_H
#define ICATEGORIESDAO_H

#include <QString>
#include <QList>
#include <QMap>

class ICategoriesDAO
{
public:
    virtual ~ICategoriesDAO() {}

    // 获取所有类别（包括层次结构）
    virtual QMap<int, QString> getAllCategories() = 0;

    // 根据父类ID获取子类别
    virtual QList<QPair<int, QString>> getSubcategories(int parentId) = 0;

    // 添加新类别
    virtual bool addCategory(const QString& name, int parentId) = 0;

    // 删除类别
    virtual bool deleteCategory(int categoryId) = 0;
};

#endif // ICATEGORIESDAO_H
