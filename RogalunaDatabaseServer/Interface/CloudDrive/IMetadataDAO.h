#ifndef IMETADATADAO_H
#define IMETADATADAO_H

#include <QDateTime>
#include <QString>

struct FileMetadata {
    QString uid;
    int userId;
    QString fileName;
    bool isDirectory;
    QString parentUid;
    int version;
    QDateTime uploadedAt;
};

class IMetadataDAO
{
public:
    virtual ~IMetadataDAO() {}

    virtual bool insertMetadata(const QString& uid, int userId, const QString& fileName, bool isDirectory, const QString& parentUid = QString(), int version = 1) = 0;
    virtual std::optional<FileMetadata> getUserFile(const QString& uid, int userId) = 0;
    virtual bool deleteMetadata(const QString& uid) = 0;
};

#endif // IMETADATADAO_H
