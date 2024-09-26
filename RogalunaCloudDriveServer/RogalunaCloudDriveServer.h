#ifndef ROGALUNACLOUDDRIVESERVER_H
#define ROGALUNACLOUDDRIVESERVER_H

#include "RogalunaCloudDriveServer_global.h"

#include <RogalunaStorageServer.h>

class ROGALUNACLOUDDRIVESERVER_EXPORT RogalunaCloudDriveServer
{
public:
    RogalunaCloudDriveServer(RogalunaStorageServer* storageServer, const QString &root);

public:
    QVector<FileInfoStruct> getDirFiles(const QString &driveName, const QString &targetPath);

private:


private:
    RogalunaStorageServer* storageServer;
    QString root;
};

#endif // ROGALUNACLOUDDRIVESERVER_H
