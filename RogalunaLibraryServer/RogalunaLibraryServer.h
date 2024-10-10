#ifndef ROGALUNALIBRARYSERVER_H
#define ROGALUNALIBRARYSERVER_H

#include "RogalunaLibraryServer_global.h"

#include <RogalunaDatabaseServer.h>
#include <RogalunaStorageServer.h>

#include <Database/Library/CategoriesDAO.h>

class ROGALUNALIBRARYSERVER_EXPORT RogalunaLibraryServer
{
public:
    RogalunaLibraryServer(RogalunaStorageServer* storageServer, RogalunaDatabaseServer* databaseServer, const QString &root, int categoryRootId = 1);

public:
    // Storage
    QVector<FileInfoStruct> getDirFiles(const QString &driveName, const QString &targetPath);

    // Database
    QJsonObject getCategories(const QString& parentCategoryName = "");

private:
    QJsonObject buildCategoryJson(Library::CategoriesDAO &dao, int parentId);

private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
    QString root;
    int categoryRootId;
};

#endif // ROGALUNALIBRARYSERVER_H
