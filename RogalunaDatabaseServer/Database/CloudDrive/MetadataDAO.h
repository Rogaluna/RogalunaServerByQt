#ifndef METADATADAO_H
#define METADATADAO_H

#include <BaseDAO.h>
#include <Interface/CloudDrive/IMetadataDAO.h>

namespace CloudDrive {

class ROGALUNADATABASESERVER_EXPORT MetadataDAO : public BaseDAO , public IMetadataDAO {
public:
    explicit MetadataDAO(QSqlDatabase& db, const QString& schema = "cloud_drive")
        : BaseDAO(db, schema, "metadata") {}

public:
    // 实现 IMetadataDAO 接口的方法
    QString insertMetadata(int userId, const QString& fileName, const QString &contentMd5, bool isDirectory, const QString& parentUid = QString(), int version = 1) override;

    bool deleteMetadata(const QString& uid) override;

    QString getUserRootDirUid(int userId) override;
    QString insertFolder(int userId, const QString &parentUid, const QString &folderName) override;
    QString getPath(const QString& uid) override;

    std::optional<FFileMetadata> getMetadataFromPath(const QString& path, int userId) override;

    std::optional<QVector<FFileMetadata>> getMetadataByUserId(int userId) override;

    std::optional<QVector<FFileMetadata>> getMetadataByUid(const QString &uid) override;

    std::optional<QVector<FFileMetadata>> getMetadataUnderFolder(const QString &folderUid) override;

    bool updateFileName(const QString& uid, const QString &name) override;

    int getRefCount(const QString& field, const QString &value) override;
};

}
#endif // METADATADAO_H
