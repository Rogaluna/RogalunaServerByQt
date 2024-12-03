#ifndef RESOURCESCOUNTDAO_H
#define RESOURCESCOUNTDAO_H

#include <BaseDAO.h>

#include <Interface/Library/IResourcesCountDAO.h>

namespace Library {

class ROGALUNADATABASESERVER_EXPORT ResourcesCountDAO : public BaseDAO, public IResourcesCountDAO
{
public:
    explicit ResourcesCountDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "resources_count") {}

public:
    // 获取资源计数
    int getResourceCount(const QString &resId) override;

    // 插入资源行
    bool insertResourceCount(const QString &resId, int count) override;

    // 删除资源行
    bool deleteResourceCount(const QString &resId) override;

    // 修改资源计数
    bool modifyResourceCount(const QString &resId, int count) override;
};

}

#endif // RESOURCESCOUNTDAO_H
