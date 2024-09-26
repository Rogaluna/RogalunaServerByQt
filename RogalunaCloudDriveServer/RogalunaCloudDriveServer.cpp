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
        rootDir.mkpath(QDir::separator());  // 创建根文件夹
    }
}

bool RogalunaCloudDriveServer::uploadChunk(const QString tempDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5)
{
    // 将块数据写入临时文件夹
    const QString &targetPath = storageServer->absoluteFilePath(tempDirName + QDir::separator() + QString::number(chunkIndex), storageServer->temp);
    return storageServer->writeFile(targetPath, chunkData, chunkMd5);
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
    // 读取文件内容
    return storageServer->readFile(
        storageServer->absoluteFilePath(
            root + QDir::separator() +
            contentMd5),
        0);
}

bool RogalunaCloudDriveServer::deleteFile(const QString &uid, const QString& userId)
{
    return databaseServer->executeTransaction([&]() {
        CloudDrive::ContentDAO contentDao(databaseServer->getDatabase());
        CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());

        // 根据 uid 获取记录
        const std::optional<QVector<CloudDrive::FFileMetadata>> &records = metadataDao.getMetadataByUid(uid);
        if (!records.has_value() || records->isEmpty()) {
            qWarning() << "Can't find records.";
            return false;
        }
        const CloudDrive::FFileMetadata &record = records->first();
        if (!userId.isEmpty()) {
            // userId 不为空，启用权限检查
            if (record.userId != userId.toInt()) {
                // 权限不匹配

                return false;
            }
        }

        const QString &contentMd5 = record.contentMd5;
        bool isDir = record.isDirectory;

        // 如果指向的仅是一个单纯的文件
        if (!isDir) {
            // 查找对应 md5 值在数据库中的引用记录是否仅存一个
            int count = metadataDao.getRefCount("content_md5", contentMd5);
            if (!count) {
                qWarning() << "Count has a impossible value.";
                return false;
            }

            // 如果有多个记录指向此 md5 值，那么仅删除元数据中的记录
            if (count > 1) {
                // 删除数据库中的元数据记录
                if (!metadataDao.deleteMetadata(uid)) {
                    qWarning() << "Failed to delete metadata.";
                    return false;
                }
            }

            // 否则删除记录的同时，将存储的文件也一并删除
            else {
                // 删除数据库中的元数据记录
                if (!metadataDao.deleteMetadata(uid)) {
                    qWarning() << "Failed to delete metadata.";
                    return false;
                }

                // 删除数据库中的内容记录
                if (!contentDao.deleteContent(contentMd5)) {
                    qWarning() << "Failed to delete file content.";
                    return false;
                }

                // 删除文件
                if (!storageServer->deleteFile(storageServer->absoluteFilePath(
                        root + QDir::separator() + contentMd5))) {
                    qWarning() << "Failed to delete file.";
                    return false;
                }
            }

            return true;
        }

        // 如果指向的是一个文件夹，那么还需要查找其内部包含的所有文件
        else {
            // 获取此目录下包含的所有子 uid

            // 根据 uid 获取记录

            // 如果记录表示的是目录，再进行查找

            // 删除数据库中的元数据记录
            if (!metadataDao.deleteMetadata(uid)) {
                qWarning() << "Failed to delete metadata.";
                return false;
            }

            return true;
        }

    });
}

std::optional<QVector<CloudDrive::FFileMetadata>> RogalunaCloudDriveServer::getFiles(const QString &query, const EGetFileOpterator &Operator)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    switch (Operator) {
    case EGetFileOpterator::E_UID: // 通过 uid 查询
    {
        return metadataDao.getMetadataByUid(query);
    }
        break;
    case EGetFileOpterator::E_USERID: // 通过 userId 查询
    {
        return metadataDao.getMetadataByUserId(query.toInt());
    }
        break;
    case EGetFileOpterator::E_FOLDER: // 通过 文件夹uid 查询
    {
        return metadataDao.getMetadataUnderFolder(query);
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
    QVector<CloudDrive::FFileMetadata> uidObjects = metadataDao.getMetadataByUid(uid).value_or(QVector<CloudDrive::FFileMetadata>());

    if (uidObjects.isEmpty()) {
        // 没有找到对应的 uid 对象，无法寻找到其父对象
        return std::nullopt;
    }

    const CloudDrive::FFileMetadata &uidObject = uidObjects[0];

    QVector<CloudDrive::FFileMetadata> parentObjects = metadataDao.getMetadataByUid(uidObject.parentUid).value_or(QVector<CloudDrive::FFileMetadata>());

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

bool RogalunaCloudDriveServer::getTargetFile(const QString &targetMd5, bool isRangeRequest, QByteArray &fileData, qint64 &startPos, qint64 &endPos, qint64 &fileSize)
{
    // 获取文件路径
    QString filePath = storageServer->absoluteFilePath(root + QDir::separator() + targetMd5);
    QFile file(filePath);

    // 检查文件是否存在并尝试打开
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // 获取文件大小
    fileSize = file.size();

    if (isRangeRequest) {
        // 假设 startPos 和 endPos 已由调用者解析并传递
        // 那么不再解析 Range 头，而是使用传入的 startPos 和 endPos
        // 需要确保调用者已经正确设置了 startPos 和 endPos
        if (startPos > endPos || startPos >= fileSize) {
            // 无效的范围
            return false;
        }
        // 修正 endPos 不超过文件大小
        if (endPos >= fileSize) {
            endPos = fileSize - 1;
        }
    } else {
        // 非 Range 请求，读取整个文件
        startPos = 0;
        endPos = fileSize - 1;
    }

    // 设置文件指针位置并读取数据
    file.seek(startPos);
    qint64 bytesToRead = endPos - startPos + 1;
    fileData = file.read(bytesToRead);

    // 检查读取是否成功
    if (fileData.size() != bytesToRead) {
        return false;
    }

    return true;
}

bool RogalunaCloudDriveServer::renameFile(const QString &uid, const QString &newName)
{
    CloudDrive::MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.updateFileName(uid, newName);
}

