#include "RogalunaCloudDriveServer.h"

#include <QDir>

RogalunaCloudDriveServer::RogalunaCloudDriveServer(RogalunaStorageServer* storageServer, const QString &root)
    : storageServer(storageServer)
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
