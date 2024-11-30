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

    // 增加资源计数
    virtual QPair<bool, QStringList> addResourceCount(const QStringList &added) = 0;

    // 减少资源计数
    virtual QPair<bool, QStringList> removeResourceCount(const QStringList &removed) = 0;
};

}

#endif // IRESOURCESCOUNTDAO_H
