#include "RogalunaCloudDriveServer.h"

#include <QDir>
#include <QCryptographicHash>

#include <Database/CloudDrive/ContentDAO.h>
#include <Database/CloudDrive/MetadataDAO.h>

RogalunaCloudDriveServer::RogalunaCloudDriveServer(RogalunaStorageServer* storageServer, RogalunaDatabaseServer* databaseServer, const QString &root)
    : storageServer(storageServer)
    , databaseServer(databaseServer)
    , root(root)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(root);  // 创建根文件夹
    }
}

bool RogalunaCloudDriveServer::uploadChunk(const QString tempDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5)
{
    // 将块数据写入临时文件夹
    return storageServer->writeTempFile(tempDirName, chunkIndex, chunkData, chunkMd5);
}

QString RogalunaCloudDriveServer::createFolder(int userId, const QString &parentUid, const QString &folderName)
{
    MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.insertFolder(userId, parentUid, folderName);
}

QString RogalunaCloudDriveServer::mergeChunks(const QString &tempDirName, const QString &fileName, int totalChunks, int userId, const QString &parentUid)
{
    // 需要进行合并的文件夹名称即是文件的 MD5 值
    const QString& targetMd5 = tempDirName;

    // 目标文件存储路径
    QString targetName = root + "/" + targetMd5;

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

    MetadataDAO metadataDao(databaseServer->getDatabase());
    ContentDAO contentDao(databaseServer->getDatabase());

    // 插入文件内容记录
    if (!contentDao.insertContent(targetMd5, mergedFile.size())) {
        qWarning() << "Failed to insert file content into database";
        return QString();
    }

    // 插入文件元数据记录, uuid 由数据库自动生成
    QString uuid = metadataDao.insertMetadata(userId, fileName, targetMd5, false, parentUid);
    if (uuid.isEmpty()) {
        qWarning() << "Failed to insert file metadata into database";
        return QString();
    }

    return uuid;
}

FileReadResult RogalunaCloudDriveServer::downloadFile(const QString &contentMd5)
{
    ContentDAO contentDao(databaseServer->getDatabase());

    // 从数据库中获取文件的存储路径
    QString filePath = contentDao.getContentPath(contentMd5);
    if (filePath.isEmpty()) {
        return { QByteArray(), false, "File not found", true };
    }

    // 打开文件进行读取
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return { QByteArray(), false, "Failed to open file", true };
    }

    // 读取文件内容
    FileReadResult result = storageServer->readFile(file);
    file.close(); // 关闭文件
    return result;
}

bool RogalunaCloudDriveServer::deleteFile(const QString &contentMd5)
{
    ContentDAO contentDao(databaseServer->getDatabase());
    MetadataDAO metadataDao(databaseServer->getDatabase());

    // 从数据库中获取文件的存储路径
    QString filePath = contentDao.getContentPath(contentMd5);
    if (filePath.isEmpty()) {
        qWarning() << "File not found in database:" << contentMd5;
        return false;
    }

    // 删除文件
    if (!storageServer->deleteFile(filePath)) {
        qWarning() << "Failed to delete file:" << filePath;
        return false;
    }

    // 删除数据库中的文件记录
    if (!contentDao.deleteContent(contentMd5) || !metadataDao.deleteMetadata(contentMd5)) {
        qWarning() << "Failed to delete file metadata from database";
        return false;
    }

    return true;
}

std::optional<QVector<FileMetadata>> RogalunaCloudDriveServer::getFiles(const QString &query, const EGetFileOpterator &Operator)
{
    MetadataDAO metadataDao(databaseServer->getDatabase());
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

QString RogalunaCloudDriveServer::getUserRootDirUid(int userId)
{
    MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.getUserRootDirUid(userId);
}

