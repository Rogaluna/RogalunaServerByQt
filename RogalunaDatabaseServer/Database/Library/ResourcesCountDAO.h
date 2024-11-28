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

    // 获取资源类型
    QString getResourceType(const QString &resId) override;
};

}

#endif // RESOURCESCOUNTDAO_H
