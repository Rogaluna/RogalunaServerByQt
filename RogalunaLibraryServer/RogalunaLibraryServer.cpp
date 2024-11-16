#include "RogalunaLibraryServer.h"

#include <QJsonArray>
#include <QJsonObject>

#include <Database/Library/BookCategoriesDAO.h>
#include <Database/Library/BooksDAO.h>
#include <Database/Library/CategoriesDAO.h>
#include <Database/Library/ChaptersDAO.h>
#include <Database/Library/ReadProgressDAO.h>
#include <Database/Library/UserCollectionsDAO.h>

RogalunaLibraryServer::RogalunaLibraryServer(RogalunaStorageServer *storageServer, RogalunaDatabaseServer *databaseServer, const QString &root, int categoryRootId)
    : root(root)
    , storageServer(storageServer)
    , databaseServer(databaseServer)
    , categoryRootId(categoryRootId)
    , maxRangeSize(10)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(".");  // 创建根文件夹
    }
}

QVector<FileInfoStruct> RogalunaLibraryServer::getDirFiles(const QString &driveName, const QString &targetPath)
{
    QString dirPath = root \
                      + (driveName.startsWith(QDir::separator()) ? QChar::Space : QDir::separator()) + driveName \
                      + (targetPath.startsWith(QDir::separator()) ? QChar::Space : QDir::separator()) + targetPath;

    return storageServer->listFiles(dirPath);
}

QJsonObject RogalunaLibraryServer::getCategories(int parentCategoryId)
{
    // 获取数据库连接（假设有一个方法可以获取QSqlDatabase）
    Library::CategoriesDAO categoriesDAO(databaseServer->getDatabase());
    return categoriesDAO.getSubcategories(parentCategoryId);
}

QString RogalunaLibraryServer::registerBook(const QString &userId, const QString &bookName, const QString &bookDesc, const QVector<int> &tags)
{
    QString bookId;

    bool bReg = databaseServer->executeTransaction([&]() {
        // 将书籍写入书籍表
        Library::BooksDAO booksDAO(databaseServer->getDatabase());
        bookId = booksDAO.addBook(userId, bookName, bookDesc);
        if (bookId.isEmpty()) {
            return false;
        }

        // 将标签注册到标签映射表
        Library::BookCategoriesDAO bookCategoriesDAO(databaseServer->getDatabase());
        if (!bookCategoriesDAO.addBookTags(bookId, tags)) {
            return false;
        }

        return true;
    });

    if (!bReg) {
        // 如果书籍注册失败
    }

    return bookId;
}

QJsonArray RogalunaLibraryServer::getBookList(int opt, QString param)
{
    Library::FGetBookOpt eOpt;
    if (opt >= static_cast<int>(Library::FGetBookOpt::E_NORMAL) && opt <= static_cast<int>(Library::FGetBookOpt::E_USER_WRITE)) {
        eOpt = static_cast<Library::FGetBookOpt>(opt);
    } else {
        return QJsonArray(); // 枚举越界，直接返回空数组
    }

    switch (eOpt) {
    case Library::FGetBookOpt::E_NORMAL:
    {
        Library::BooksDAO booksDAO(databaseServer->getDatabase());
        return booksDAO.getBooksByRandom(maxRangeSize);
    }
    break;
    case Library::FGetBookOpt::E_USER_COLLECTION:
    {
        Library::UserCollectionsDAO userCollectionsDAO(databaseServer->getDatabase());
        QStringList bookIds = userCollectionsDAO.getBooksByUserCollection(param);

        Library::BooksDAO booksDAO(databaseServer->getDatabase());
        return booksDAO.getBooksByIds(bookIds);
    }
    break;
    case Library::FGetBookOpt::E_USER_WRITE:
    {
        Library::BooksDAO booksDAO(databaseServer->getDatabase());
        return booksDAO.getBooksByUserWrite(param, maxRangeSize);
    }
    break;
    default:
        return QJsonArray();
    }
}

QJsonObject RogalunaLibraryServer::getBookInfo(const QString &bookId)
{
    Library::BooksDAO booksDAO(databaseServer->getDatabase());
    return booksDAO.getBookDetails(bookId);
}

QJsonObject RogalunaLibraryServer::getBookCategories(const QString &bookId)
{
    Library::BookCategoriesDAO bookCategoriesDAO(databaseServer->getDatabase());
    return bookCategoriesDAO.getBookTags(bookId);
}

QJsonArray RogalunaLibraryServer::getChapterList(const QString &bookId)
{
    Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
    return chaptersDAO.listChapters(bookId);
}

QJsonObject RogalunaLibraryServer::getChapterInfo(const QString &bookId, const QString &chapterIndex)
{
    Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
    return chaptersDAO.getChapterDetails(bookId, chapterIndex.toInt());
}

bool RogalunaLibraryServer::registerChapter(const QString &bookId, int chapterIndex, const QString &chapterName, const QString &groupName, const QString &fileName)
{
    return databaseServer->executeTransaction([&]() {
        // 数据库中注册章节信息
        Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
        if (!chaptersDAO.addChapter(bookId, chapterIndex, chapterName, groupName, fileName)) {
            return false;
        }

        // 文件系统中创建对应空白文件
        if (!storageServer->writeFile(
                storageServer->absoluteFilePath(
                    root + QDir::separator() +
                    fileName),
                QByteArray())) {
            return false;
        }

        return true;
    });
}

QJsonObject RogalunaLibraryServer::getBookReadProgress(const QString &bookId, const QString &userId)
{
    Library::ReadProgressDAO readProgressDAO(databaseServer->getDatabase());
    return readProgressDAO.getProgress(userId.toInt(), bookId);
}

bool RogalunaLibraryServer::updateBookReadProgress(const QString &bookId, const QString &userId, const QString &chapterIndex, const QString &percentProgress)
{
    Library::ReadProgressDAO readProgressDAO(databaseServer->getDatabase());
    return readProgressDAO.updateProgress(userId.toInt(), bookId, chapterIndex.toInt(),percentProgress.toInt());
}

bool RogalunaLibraryServer::updateChapterContent(const QString &bookId, const QString &chapterIndex, const QString &chapterContent)
{
    return databaseServer->executeTransaction([&]() {
        Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
        // 获取对应的文件名称
        QJsonObject result = chaptersDAO.getChapterDetails(bookId, chapterIndex.toInt());
        if (result.isEmpty()) {
            return false;
        }
        const QString &fileName = result["file_name"].toString();

        // 更新文件内容
        if(!storageServer->writeFile(
                storageServer->absoluteFilePath(
                    root + QDir::separator() +
                    fileName),
                chapterContent.toUtf8())) {
            return false;
        }

        return true;
    });
}

bool RogalunaLibraryServer::updateBookInfo(const QString &bookId, const QString &newName, const QString &newDesc, const QVector<int> &newTags)
{
    return databaseServer->executeTransaction([&]() {
        Library::BooksDAO booksDAO(databaseServer->getDatabase());
        if (!booksDAO.updateBook(bookId, newName, newDesc)) {
            return false;
        }

        Library::BookCategoriesDAO bookCategoriesDAO(databaseServer->getDatabase());
        if (!bookCategoriesDAO.deleteAllBookTags(bookId)) {
            return false;
        }

        if (!bookCategoriesDAO.addBookTags(bookId, newTags)) {
            return false;
        }

        return true;
    });
}

bool RogalunaLibraryServer::updateChapterInfo(const QString &bookId, const QString &chapterIndex, const QString &newIndex, const QString &newName, const QString &newGroup)
{
    Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
    return chaptersDAO.updateChapter(bookId, chapterIndex.toInt(), newIndex.toInt(), newName, newGroup, 0);
}

bool RogalunaLibraryServer::deleteChapter(const QString &bookId, const QString &chapterIndex)
{
    return databaseServer->executeTransaction([&]() {
        Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
        // 获取对应的文件名称
        QJsonObject result = chaptersDAO.getChapterDetails(bookId, chapterIndex.toInt());
        if (result.isEmpty()) {
            return false;
        }
        const QString &fileName = result["file_name"].toString();

        // 注销记录的章节信息
        if(!chaptersDAO.deleteChapter(bookId, chapterIndex.toInt())){
            return false;
        }

        // 将文件删除
        if(!storageServer->deleteFile(storageServer->absoluteFilePath(
                root + QDir::separator() +
                fileName))) {
            return false;
        }

        return true;
    });
}

bool RogalunaLibraryServer::deleteBook(const QString &bookId)
{
    return databaseServer->executeTransaction([&]() {
        // 获取该书所有的章节文件名称
        Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
        QPair<QVector<QString>, bool> result = chaptersDAO.getAllChapterFileName(bookId);
        const QVector<QString> &fileNames = result.first;
        const bool &bSuccess = result.second;
        if (!bSuccess) {
            return false;
        }

        // 注销书籍
        Library::BooksDAO booksDAO(databaseServer->getDatabase());
        if (!booksDAO.deleteBook(bookId)) {
            return false;
        }

        // 删除所有的章节文件
        for (const QString &fileName : fileNames) {
            if(!storageServer->deleteFile(storageServer->absoluteFilePath(
                    root + QDir::separator() +
                    fileName))) {
                return false;
            }
        }

        return true;
    });
}

