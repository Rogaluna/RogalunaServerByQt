#include "RogalunaCloudDriveServer.h"

#include <QDir>
#include <QCryptographicHash>

#include <Database/CloudDrive/ContentDAO.h>
#include <Database/CloudDrive/MetadataDAO.h>

RogalunaCloudDriveServer::RogalunaCloudDriveServer(RogalunaStorageServer* storageServer, RogalunaDatabaseServer* databaseServer, const QString &root)
    : root(root)
    , storageServer(storageServer)
    , databaseServer(databaseServer)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(".");  // 创建根文件夹
    }
}

bool RogalunaCloudDriveServer::uploadChunk(const QString tempDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5)
{
    // 将块数据写入临时文件夹
    return storageServer->writeTempFile(tempDirName, chunkIndex, chunkData, chunkMd5);
}

QString RogalunaCloudDriveServer::createFolder(int userId, const QString &parentUid, const QString &folderName)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.insertFolder(userId, parentUid, folderName);
}

QString RogalunaCloudDriveServer::mergeChunks(const QString &tempDirName, const QString &fileName, int totalChunks, int userId, const QString &parentUid)
{
    QString uuid;

    // 需要进行合并的文件夹名称即是文件的 MD5 值
    const QString& targetMd5 = tempDirName;

    // 目标文件存储路径
    QString targetName = root + QDir::separator() + targetMd5;

    // 合并临时文件夹中的文件块
    if (!storageServer->mergeTempFile(tempDirName, totalChunks, root, targetMd5)) {
        qWarning() << "Failed to merge chunks";
        return QString();
    }

    QFile mergedFile(storageServer->absoluteFilePath(targetName));
    if (!mergedFile.exists()) {
        qWarning() << "Merged file not found";
        return QString();
    }

    bool bReg = databaseServer->executeTransaction([&]() {
        CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
        CloudDrive::ContentDAO contentDao(databaseServer->getDatabase());

        // 插入文件内容记录
        if (!contentDao.insertContent(targetMd5, mergedFile.size())) {
            qWarning() << "Failed to insert file content into database";
            return false;
        }

        // 插入文件元数据记录, uuid 由数据库自动生成
        uuid = metadataDao.insertMetadata(userId, fileName, targetMd5, false, parentUid);
        if (uuid.isEmpty()) {
            qWarning() << "Failed to insert file metadata into database";
            return false;
        }

        return true;
    });

    if (!bReg) {
        // 如果数据库注册失败，检查之前合并的文件，进行清理
        if (mergedFile.exists()) {
            mergedFile.remove();
        }
    }

    return uuid;
}

FileReadResult RogalunaCloudDriveServer::downloadFile(const QString &contentMd5)
{
    CloudDrive::ContentDAO contentDao(databaseServer->getDatabase());

    // 从数据库中获取文件的存储路径
    QString filePath = contentDao.getContentPath(contentMd5);
    if (filePath.isEmpty()) {
        return { QByteArray(), false, "File not found", true };
    }

    // 读取文件内容
    FileReadResult result = storageServer->readFile(filePath, 0);
    return result;
}

bool RogalunaCloudDriveServer::deleteFile(const QString &contentMd5)
{
    return databaseServer->executeTransaction([&]() {
        CloudDrive::ContentDAO contentDao(databaseServer->getDatabase());
        CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());

        // 从数据库中获取文件的存储路径
        QString filePath = contentDao.getContentPath(contentMd5);
        if (filePath.isEmpty()) {
            qWarning() << "File not found in database:" << contentMd5;
            return false;
        }

        // 删除数据库中的文件记录
        if (!contentDao.deleteContent(contentMd5)) {
            qWarning() << "Failed to delete file content.";
            return false;
        }

        if (!metadataDao.deleteMetadata(contentMd5)) {
            qWarning() << "Failed to delete file metadata.";
            return false;
        }

        // 删除文件
        if (!storageServer->deleteFile(filePath)) {
            qWarning() << "Failed to delete file:" << filePath;
            return false;
        }

        return true;
    });
}

std::optional<QVector<CloudDrive::FFileMetadata>> RogalunaCloudDriveServer::getFiles(const QString &query, const EGetFileOpterator &Operator)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    switch (Operator) {
    case EGetFileOpterator::E_UID: // 通过 uid 查询
    {
        return metadataDao.getUidFile(query);
    }
        break;
    case EGetFileOpterator::E_USERID: // 通过 userId 查询
    {
        return metadataDao.getUserFiles(query.toInt());
    }
        break;
    case EGetFileOpterator::E_FOLDER: // 通过 文件夹uid 查询
    {
        return metadataDao.getFolderFiles(query);
    }
        break;
    default:
        return std::nullopt;
    }
}

QString RogalunaCloudDriveServer::getPath(const QString &uid)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.getPath(uid);
}

QString RogalunaCloudDriveServer::getUserRootDirUid(int userId)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.getUserRootDirUid(userId);
}

std::optional<CloudDrive::FFileMetadata> RogalunaCloudDriveServer::getParent(const QString &uid)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    QVector<CloudDrive::FFileMetadata> uidObjects = metadataDao.getUidFile(uid).value_or(QVector<CloudDrive::FFileMetadata>());

    if (uidObjects.isEmpty()) {
        // 没有找到对应的 uid 对象，无法寻找到其父对象
        return std::nullopt;
    }

    const CloudDrive::FFileMetadata &uidObject = uidObjects[0];

    QVector<CloudDrive::FFileMetadata> parentObjects = metadataDao.getUidFile(uidObject.parentUid).value_or(QVector<CloudDrive::FFileMetadata>());

    if (parentObjects.isEmpty()) {
        // 没有找到任何父对象，说明它本身就是根目录
        return std::nullopt;
    }

    return parentObjects[0];
}

std::optional<CloudDrive::FFileMetadata> RogalunaCloudDriveServer::getMetadataFromPath(const QString &path, int userId)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.getMetadataFromPath(path, userId);
}

