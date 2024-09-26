#include "RogalunaLibraryServer.h"

#include <QJsonObject>

#include <Database/Library/CategoriesDAO.h>

RogalunaLibraryServer::RogalunaLibraryServer(RogalunaStorageServer *storageServer, RogalunaDatabaseServer *databaseServer, const QString &root, int categoryRootId)
    : storageServer(storageServer)
    , databaseServer(databaseServer)
    , root(root)
    , categoryRootId(categoryRootId)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(root);  // 创建根文件夹
    }
}

QVector<FileInfoStruct> RogalunaLibraryServer::getDirFiles(const QString &driveName, const QString &targetPath)
{
    QString dirPath = root \
                      + (driveName.startsWith('/') ? "" : "/") + driveName \
                      + (targetPath.startsWith('/') ? "" : "/") + targetPath;

    return storageServer->listFiles(dirPath);
}

QJsonObject RogalunaLibraryServer::getCategories(const QString& parentCategoryName)
{
    QJsonObject result;

    // 获取数据库连接（假设有一个方法可以获取QSqlDatabase）
    CategoriesDAO categoriesDAO(databaseServer->getDatabase());

    // 查找从哪个父类别开始（默认为根类别，即 parent_id = NULL）
    int parentId = categoryRootId;
    if (!parentCategoryName.isEmpty()) {
        // 查找父类别的ID
        QMap<int, QString> allCategories = categoriesDAO.getAllCategories();
        auto it = allCategories.key(parentCategoryName);
        if (it != categoryRootId) {
            parentId = it;
        } else {
            qWarning() << "Category not found for name:" << parentCategoryName;
            return result;
        }
    }

    // 递归构建类别层次的 JSON
    result = buildCategoryJson(categoriesDAO, parentId);

    return result;
}

// 递归构建类别层次 JSON 的辅助函数
QJsonObject RogalunaLibraryServer::buildCategoryJson(CategoriesDAO& dao, int parentId)
{
    QJsonObject categoryObject;
    QList<QPair<int, QString>> subcategories = dao.getSubcategories(parentId);

    for (const auto& subcategory : subcategories) {
        QJsonObject subCategoryJson;
        subCategoryJson["name"] = subcategory.second;

        // 递归添加子类别
        subCategoryJson["subcategories"] = buildCategoryJson(dao, subcategory.first);
        categoryObject[QString::number(subcategory.first)] = subCategoryJson;
    }

    return categoryObject;
}
