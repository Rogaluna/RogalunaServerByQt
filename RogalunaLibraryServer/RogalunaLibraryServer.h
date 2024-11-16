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

    // 获取类别
    QJsonObject getCategories(int parentCategoryId = 0);

    // 注册新书籍，返回注册书籍的 id
    QString registerBook(const QString &userId, const QString &bookName, const QString &bookDesc, const QVector<int> &tags);

    // 获取书籍列表
    QJsonArray getBookList(int opt, QString param);

    // 获取书籍信息
    QJsonObject getBookInfo(const QString &bookId);

    // 获取书籍类别
    QJsonObject getBookCategories(const QString &bookId);

    // 获取书籍的章节信息
    QJsonArray getChapterList(const QString &bookId);

    // 获取章节信息
    QJsonObject getChapterInfo(const QString &bookId, const QString &chapterIndex);

    // 注册新章节，会创建一个 0 字节的文件到文件系统
    bool registerChapter(const QString &bookId, int chapterIndex, const QString &chapterName, const QString &groupName, const QString &fileName);

    // 获取用户选择的书籍进度
    QJsonObject getBookReadProgress(const QString &bookId, const QString &userId);

    // 更新用户阅读的书籍进度
    bool updateBookReadProgress(const QString &bookId, const QString &userId, const QString &chapterIndex, const QString &percentProgress);

    // 更新章节内容
    bool updateChapterContent(const QString &bookId, const QString &chapterIndex, const QString &chapterContent);

    // 更新书籍信息
    bool updateBookInfo(const QString &bookId, const QString &newName, const QString &newDesc, const QVector<int> &newTags);

    // 更新章节信息
    bool updateChapterInfo(const QString &bookId, const QString &chapterIndex, const QString &newIndex, const QString &newName, const QString &newGroup);

    // 删除章节
    bool deleteChapter(const QString &bookId, const QString &chapterIndex);

    // 删除书籍
    bool deleteBook(const QString &bookId);

public:
    QString root;                    ///< 文件存储的根目录。

private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
    int categoryRootId;

    int maxRangeSize;
};

#endif // ROGALUNALIBRARYSERVER_H
