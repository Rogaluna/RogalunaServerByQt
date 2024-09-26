#ifndef ICATEGORIESDAO_H
#define ICATEGORIESDAO_H

#include <QJsonArray>

namespace Library {

class ICategoriesDAO
{
public:
    virtual ~ICategoriesDAO() {}

    // 获取所有类别（包括层次结构）
    virtual QJsonObject getSubcategories(int parentId) = 0;

    // 添加新类别
    virtual bool addCategory(const QString& name, int parentId) = 0;

    // 删除类别
    virtual bool deleteCategory(int categoryId) = 0;
};

}

#endif // ICATEGORIESDAO_H
