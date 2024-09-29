#ifndef ROGALUNACLOUDDRIVESERVER_H
#define ROGALUNACLOUDDRIVESERVER_H

#include "RogalunaCloudDriveServer_global.h"

#include <RogalunaStorageServer.h>
#include <RogalunaDatabaseServer.h>

#include <Interface/CloudDrive/IMetadataDAO.h>

class ROGALUNACLOUDDRIVESERVER_EXPORT RogalunaCloudDriveServer
{
public:
    RogalunaCloudDriveServer(RogalunaStorageServer* storageServer, RogalunaDatabaseServer* databaseServer, const QString &root);

public:
    enum class EGetFileOpterator : char {
        E_UID,
        E_USERID,
        E_FOLDER,
    };

public:
    // 上传文件块
    bool uploadChunk(const QString tempDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5 = "");

    // 插入文件夹，返回文件的 uid
    QString createFolder(int userId, const QString &parentUid, const QString &folderName);

    // 合并文件块，返回文件的 uid
    QString mergeChunks(const QString &tempDirName, const QString &fileName, int totalChunks, int userId, const QString &parentUid = QString());

    // 下载文件，通过 MD5 获取文件数据
    FileReadResult downloadFile(const QString &contentMd5);

    // 删除文件，通过 MD5 删除文件和数据库记录
    bool deleteFile(const QString &contentMd5);

    // 获取某个用户拥有的文件（选项：特定uid、特定userId、特定文件夹uid）
    std::optional<QVector<FileMetadata>> getFiles(const QString &query, const EGetFileOpterator &Operator);
    QString getUserRootDirUid(int userId);


private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
    QString root;
};

#endif // ROGALUNACLOUDDRIVESERVER_H
