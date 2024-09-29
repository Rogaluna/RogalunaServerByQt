#ifndef IMETADATADAO_H
#define IMETADATADAO_H

#include <QDateTime>
#include <QString>

struct FileMetadata {
    QString uid;
    int userId;
    QString fileName;
    QString contentMd5;
    QString parentUid;
    QDateTime uploadedAt;
    bool isDirectory;
    int version;
    QDateTime createdAt;
    QDateTime updatedAt;
};

class IMetadataDAO
{
public:
    virtual ~IMetadataDAO() {}

    virtual QString insertMetadata(int userId, const QString& fileName, const QString &contentMd5, bool isDirectory, const QString& parentUid = QString(), int version = 1) = 0;
    virtual bool deleteMetadata(const QString& uid) = 0;
    virtual QString getUserRootDirUid(int userId) = 0;
    virtual QString insertFolder(int userId, const QString &parentUid, const QString &folderName) = 0;

    // 获取特定 user 拥有的文件/文件夹
    virtual std::optional<QVector<FileMetadata>> getUserFiles(int userId) = 0;

    // 获取特定 uid 值的文件/文件夹 （只包含一个元素的数组）
    virtual std::optional<QVector<FileMetadata>> getUidFile(const QString &uid) = 0;

    // 获取特定 folderUid（文件夹Uid） 所拥有的文件/文件夹
    virtual std::optional<QVector<FileMetadata>> getFolderFiles(const QString &folderUid) = 0;

};

#endif // IMETADATADAO_H
