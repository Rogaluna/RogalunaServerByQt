#ifndef ROGALUNALIBRARYSERVER_H
#define ROGALUNALIBRARYSERVER_H

#include "RogalunaLibraryServer_global.h"

#include <RogalunaDatabaseServer.h>
#include <RogalunaStorageServer.h>


class ROGALUNALIBRARYSERVER_EXPORT RogalunaLibraryServer
{
public:
    RogalunaLibraryServer(RogalunaStorageServer *storageServer,
                          RogalunaDatabaseServer *databaseServer,
                          const QString &root,
                          const QString &bookDirName,
                          const QString &coverDirName,
                          int maxRangeSize,
                          int maxSingleResSize,
                          int categoryRootId = 1);

public:
    // Storage

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

    // 获取书籍的章节列表
    QJsonArray getChapterList(const QString &bookId);

    // 获取章节信息
    QJsonObject getChapterInfo(const QString &bookId, const QString &chapterIndex);

    // 注册新章节，会创建一个 0 字节的文件到文件系统
    bool registerChapter(const QString &bookId, int chapterIndex, const QString &chapterName, const QString &groupName, const QString &fileName);

    // 获取章节文件，得到章节内容
    bool getChapterFile(const QString &bookId, int chapterIndex, bool isRangeRequest, QByteArray& fileData, qint64 &startPos, qint64 &endPos, qint64 &fileSize, QString &chapterName);

    // 获取用户选择的书籍进度
    QJsonObject getBookReadProgress(const QString &bookId, const QString &userId);

    // 更新用户阅读的书籍进度
    bool updateBookReadProgress(const QString &bookId, const QString &userId, const QString &chapterIndex, const QString &percentProgress);

    // 更新章节内容
    bool updateChapterContent(const QString &bookId,
                              const QString &chapterIndex,
                              const QString &chapterContent);

    // 更新书籍信息
    bool updateBookInfo(const QString &bookId, const QString &newName, const QString &newDesc, const QVector<int> &newTags);

    // 更新章节信息
    bool updateChapterInfo(const QString &bookId, const QString &chapterIndex, const QString &newIndex, const QString &newName, const QString &newGroup);

    // 删除章节
    bool deleteChapter(const QString &bookId, const QString &chapterIndex);

    // 删除书籍
    bool deleteBook(const QString &bookId);

    // 上传章节资源
    bool uplaodChapterResource(const QString &bookId, const QString &chapterName, const QString resName, const QString &type, const QByteArray &data, const QString &md5 = "");

    // 获取章节资源
    QPair<QByteArray, QString> getChapterResource(const QString &bookId, const QString &chapterName, const QString &resName);

    // 删除章节资源
    bool deleteChapterResource(const QString &bookId, const QString &chapterName, const QString &resName);

    // 上传书籍封面图片
    bool uplaodBookCover(const QString &bookId, const QString &resType, const QByteArray &data, const QString &md5 = "");

    // 获取书籍封面图片
    QPair<QByteArray, QString> getBookCover(const QString &bookId);

    // 获取被移除的资源引用
    QStringList findRemovedResources(const QString &oldHtmlContent, const QString &newHtmlContent);

private:
    QStringList extractHtmlResRef(const QString &htmlContent);

// private:
//     QStringList splitStringByLines(const QString &input);
//     QStringList splitFileByLines(const QString &filePath);

//     // Myers 行差异比较，得到新增(+)、删除(-)和修改的行
//     myersDiffCompute(const QStringList &o, const QStringList &n);

public:
    QString root;                    ///< 文件存储的根目录。
    QString bookDirName;             ///< 书籍文件目录名称。
    QString coverDirName;            ///< 封面资源目录名称。

private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
    int categoryRootId;

    int maxRangeSize;
    int maxSingleResSize;
};

#endif // ROGALUNALIBRARYSERVER_H
