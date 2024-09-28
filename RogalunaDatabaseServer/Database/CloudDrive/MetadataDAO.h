#ifndef METADATADAO_H
#define METADATADAO_H

#include <BaseDAO.h>
#include <Interface/CloudDrive/IMetadataDAO.h>

class ROGALUNADATABASESERVER_EXPORT MetadataDAO : public BaseDAO, public IMetadataDAO
{
public:
    explicit MetadataDAO(QSqlDatabase& db, const QString& schema = "cloud_drive")
        : BaseDAO(db, schema, "metadata") {}

    // 实现 IMetadataDAO 接口的方法
    bool insertMetadata(const QString& uid, int userId, const QString& fileName, bool isDirectory, const QString& parentUid = QString(), int version = 1) override;

    std::optional<FileMetadata> getUserFile(const QString& uid, int userId) override;

    bool deleteMetadata(const QString& uid) override;
};

#endif // METADATADAO_H
