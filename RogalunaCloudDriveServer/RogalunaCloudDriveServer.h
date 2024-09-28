#ifndef ROGALUNACLOUDDRIVESERVER_H
#define ROGALUNACLOUDDRIVESERVER_H

#include "RogalunaCloudDriveServer_global.h"

#include <RogalunaStorageServer.h>
#include <RogalunaDatabaseServer.h>

class ROGALUNACLOUDDRIVESERVER_EXPORT RogalunaCloudDriveServer
{
public:
    RogalunaCloudDriveServer(RogalunaStorageServer* storageServer, RogalunaDatabaseServer* databaseServer, const QString &root);

public:
    QVector<FileInfoStruct> getDirFiles(const QString &driveName, const QString &targetPath);

    // 上传文件块
    bool uploadChunk(const QByteArray &chunkData, const QString &targetMd5, int chunkIndex);

    // 合并文件块
    bool mergeChunks(const QString &targetMd5, const QString &fileName, int totalChunks, int userId, const QString &parentUid = QString());

    // 下载文件，通过 MD5 获取文件数据
    FileReadResult downloadFile(const QString &contentMd5);

    // 删除文件，通过 MD5 删除文件和数据库记录
    bool deleteFile(const QString &contentMd5);

private:
    // 计算文件的 MD5 值
    QString calculateMd5(const QByteArray &fileData);


private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
    QString root;
};

#endif // ROGALUNACLOUDDRIVESERVER_H
