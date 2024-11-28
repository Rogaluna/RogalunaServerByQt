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

    // 获取资源类型
    virtual QString getResourceType(const QString &resId) = 0;
};

}

#endif // IRESOURCESCOUNTDAO_H
