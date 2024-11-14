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

QString RogalunaLibraryServer::registerBook(const QString &userId, const QString &bookName, const QString &bookDesc, const QString &tags)
{
    // 将书籍写入书籍表
    Library::BooksDAO booksDAO(databaseServer->getDatabase());
    QString bookId = booksDAO.addBook(userId, bookName, bookDesc);

    // 将标签与书籍对应，通过标签映射表，将字符串对应到标签 id ，标签 id 与书籍 id 进行映射
    Library::CategoriesDAO categoriesDAO(databaseServer->getDatabase());
    QMap<QString, int> tagMapping = categoriesDAO.getCategoriesInversionMapping();

    // 将字符串按逗号分隔，并去除多余的空白符
    QStringList categories = tags.split(",", Qt::SkipEmptyParts);
    for (QString& category : categories) {
        category = category.trimmed();  // 去除首尾空白
    }

    // 将字符串转为 id
    QVector<int> tagIds;
    for (const QString &category : categories) {
        if (tagMapping.contains(category)) {
            tagIds.append(tagMapping[category]);  // 添加标签ID到数组
        } else {
            qWarning() << "can't find category ID:" << category;
        }
    }

    Library::BookCategoriesDAO bookCategoriesDAO(databaseServer->getDatabase());
    bool bIsAppend = bookCategoriesDAO.addBookTags(bookId, tagIds);

    if (!bIsAppend) {
        // 如果标签注册失败
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

QJsonArray RogalunaLibraryServer::getChapterList(const QString &bookId)
{
    Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
    return chaptersDAO.listChapters(bookId);
}

bool RogalunaLibraryServer::registerChapter(const QString &bookId, int chapterIndex, const QString &chapterName, const QString &groupName)
{
    // 数据库中注册章节信息
    Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
    bool bReg = chaptersDAO.addChapter(bookId, chapterIndex, chapterName, groupName);

    // 文件系统中创建对应空白文件
    bool bWrite = storageServer->writeFile(
        storageServer->absoluteFilePath(
            root + QDir::separator() +
            bookId + QDir::separator() +
            QString::number(chapterIndex)),
        QByteArray());
    return bReg && bWrite;
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
    return storageServer->writeFile(
        storageServer->absoluteFilePath(
            root + QDir::separator() +
            bookId + QDir::separator() +
            chapterIndex),
        chapterContent.toUtf8());
}

