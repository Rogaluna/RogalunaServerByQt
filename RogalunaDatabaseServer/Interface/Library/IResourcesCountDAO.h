#ifndef IRESOURCESCOUNTDAO_H
#define IRESOURCESCOUNTDAO_H

#include <QString>


namespace Library {

class IResourcesCountDAO
{
public:
    virtual ~IResourcesCountDAO() {}

public:
    // 获取资源计数
    virtual int getResourceCount(const QString &resId) = 0;

    // 修改资源计数
    virtual bool insertResourceCount(const QString &resId, int count) = 0;

    // 删除资源行
    virtual bool deleteResourceCount(const QString &resId) = 0;

    // 修改资源计数
    virtual bool modifyResourceCount(const QString &resId, int count) = 0;
};

}

#endif // IRESOURCESCOUNTDAO_H
