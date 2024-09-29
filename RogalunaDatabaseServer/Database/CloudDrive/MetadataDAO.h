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
    QString insertMetadata(int userId, const QString& fileName, const QString &contentMd5, bool isDirectory, const QString& parentUid = QString(), int version = 1) override;
    bool deleteMetadata(const QString& uid) override;
    QString getUserRootDirUid(int userId) override;
    QString insertFolder(int userId, const QString &parentUid, const QString &folderName) override;

    std::optional<QVector<FileMetadata>> getUserFiles(int userId) override;

    std::optional<QVector<FileMetadata>> getUidFile(const QString &uid) override;

    std::optional<QVector<FileMetadata>> getFolderFiles(const QString &folderUid) override;
};

#endif // METADATADAO_H
