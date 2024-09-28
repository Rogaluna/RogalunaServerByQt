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

QVector<FileInfoStruct> RogalunaCloudDriveServer::getDirFiles(const QString &driveName, const QString &targetPath)
{
    QString dirPath = root \
                      + (driveName.startsWith('/') ? "" : "/") + driveName \
                      + (targetPath.startsWith('/') ? "" : "/") + targetPath;

    return storageServer->listFiles(dirPath);
}

bool RogalunaCloudDriveServer::uploadChunk(const QByteArray &chunkData, const QString &targetMd5, int chunkIndex)
{
    // 计算块的 MD5
    QString chunkMd5 = calculateMd5(chunkData);

    // 将块数据写入临时文件夹，无需数据库操作
    if (!storageServer->writeTempFile(targetMd5, chunkIndex, chunkData, chunkMd5)) {
        qWarning() << "Failed to write chunk to temp folder";
        return false;
    }

    return true;
}

bool RogalunaCloudDriveServer::mergeChunks(const QString &targetMd5, const QString &fileName, int totalChunks, int userId, const QString &parentUid)
{
    // 目标文件存储路径
    QString targetDir = root + "/" + targetMd5;

    // 合并临时文件夹中的文件块
    if (!storageServer->mergeTempFile(targetMd5, totalChunks, targetDir, fileName)) {
        qWarning() << "Failed to merge chunks";
        return false;
    }

    QFile mergedFile(targetDir + "/" + fileName);
    if (!mergedFile.exists()) {
        qWarning() << "Merged file not found";
        return false;
    }

    MetadataDAO metadataDao(databaseServer->getDatabase());
    ContentDAO contentDao(databaseServer->getDatabase());

    // 插入文件内容记录
    if (!contentDao.insertContent(targetMd5, mergedFile.size(), QFileInfo(mergedFile).suffix(), mergedFile.fileName())) {
        qWarning() << "Failed to insert file content into database";
        return false;
    }

    // 插入文件元数据记录
    if (!metadataDao.insertMetadata(targetMd5, userId, fileName, false, parentUid)) {
        qWarning() << "Failed to insert file metadata into database";
        return false;
    }

    return true;
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

QString RogalunaCloudDriveServer::calculateMd5(const QByteArray &fileData)
{
    return QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex();
}


