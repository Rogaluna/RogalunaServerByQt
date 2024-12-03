#include "RogalunaLibraryServer.h"

#include <QJsonArray>
#include <QJsonObject>

#include <Database/Library/BookCategoriesDAO.h>
#include <Database/Library/BooksDAO.h>
#include <Database/Library/CategoriesDAO.h>
#include <Database/Library/ChaptersDAO.h>
#include <Database/Library/ReadProgressDAO.h>
#include <Database/Library/ResourcesCountDAO.h>
#include <Database/Library/UserCollectionsDAO.h>

RogalunaLibraryServer::RogalunaLibraryServer(
    RogalunaStorageServer *storageServer,
    RogalunaDatabaseServer *databaseServer,
    const QString &root,
    const QString &bookDirName,
    const QString &resDirName,
    int maxRangeSize,
    int maxSingleResSize,
    int categoryRootId)
    : root(root)
    , bookDirName(bookDirName)
    , resDirName(resDirName)
    , storageServer(storageServer)
    , databaseServer(databaseServer)
    , categoryRootId(categoryRootId)
    , maxRangeSize(maxRangeSize)
    , maxSingleResSize(maxSingleResSize)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(".");  // 创建根文件夹
    }

    QString bookDirPath = rootPath + QDir::separator() + bookDirName;
    QDir book(bookDirPath);
    if (!book.exists()) {
        book.mkpath("."); // 构造书籍目录的路径
    }

    QString resDirPath = rootPath + QDir::separator() + resDirName;
    QDir resDir(resDirPath);
    if (!resDir.exists()) {
        resDir.mkpath("."); // 构造资源目录的路径
    }
}

QString RogalunaLibraryServer::getChapterFilePath(const QString &bookId, const QString &chapterIndex)
{
    Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
    // 获取对应的文件名称
    QJsonObject result = chaptersDAO.getChapterDetails(bookId, chapterIndex.toInt());
    if (result.isEmpty()) {
        return "";
    }
    const QString &fileName = result["file_name"].toString();

    return storageServer->absoluteFilePath(
            root + QDir::separator() +
            bookDirName + QDir::separator() +
            bookId + QDir::separator() +
            fileName
        );
}

QString RogalunaLibraryServer::getResFolderPath()
{
    return root + QDir::separator() + resDirName;
}

// QVector<FileInfoStruct> RogalunaLibraryServer::getDirFiles(const QString &driveName, const QString &targetPath)
// {
//     QString dirPath = root \
//                       + (driveName.startsWith(QDir::separator()) ? QChar::Space : QDir::separator()) + driveName \
//                       + (targetPath.startsWith(QDir::separator()) ? QChar::Space : QDir::separator()) + targetPath;

//     return storageServer->listFiles(dirPath);
// }

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
                    bookDirName + QDir::separator() +
                    bookId + QDir::separator() +
                    fileName),
                QByteArray())) {
            return false;
        }

        return true;
    });
}

bool RogalunaLibraryServer::getChapterFile(const QString &bookId, int chapterIndex, bool isRangeRequest, QByteArray &fileData, qint64 &startPos, qint64 &endPos, qint64 &fileSize, QString &chapterName)
{
    Library::ChaptersDAO chaptersDAO(databaseServer->getDatabase());
    // 获取对应的文件名称
    QJsonObject result = chaptersDAO.getChapterDetails(bookId, chapterIndex);
    if (result.isEmpty()) {
        return false;
    }
    const QString &fileName = result["file_name"].toString();
    chapterName = result["title"].toString();

    // 获取文件路径
    QString filePath = storageServer->absoluteFilePath(
        root + QDir::separator() +
        bookDirName + QDir::separator() +
        bookId + QDir::separator() +
        fileName);
    QFile file(filePath);

    // 检查文件是否存在并尝试打开
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // 获取文件大小
    fileSize = file.size();

    if (isRangeRequest) {
        // 假设 startPos 和 endPos 已由调用者解析并传递
        // 那么不再解析 Range 头，而是使用传入的 startPos 和 endPos
        // 需要确保调用者已经正确设置了 startPos 和 endPos
        if (startPos > endPos || startPos >= fileSize) {
            // 无效的范围
            return false;
        }
        // 修正 endPos 不超过文件大小
        if (endPos >= fileSize) {
            endPos = fileSize - 1;
        }
    } else {
        // 非 Range 请求，读取整个文件
        startPos = 0;
        endPos = fileSize - 1;
    }

    // 设置文件指针位置并读取数据
    file.seek(startPos);
    qint64 bytesToRead = endPos - startPos + 1;
    fileData = file.read(bytesToRead);

    // 检查读取是否成功
    if (fileData.size() != bytesToRead) {
        return false;
    }

    return true;
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

bool RogalunaLibraryServer::updateChapterContent(
    const QString &bookId,
    const QString &chapterIndex,
    const QString &chapterContent)
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
                    bookDirName + QDir::separator() +
                    bookId + QDir::separator() +
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
                bookDirName + QDir::separator() +
                bookId + QDir::separator() +
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
                    bookDirName + QDir::separator() +
                    bookId + QDir::separator() +
                    fileName))) {
                return false;
            }
        }

        return true;
    });
}

int RogalunaLibraryServer::queryResCount(const QString &resId)
{
    Library::ResourcesCountDAO resourcesCountDAO(databaseServer->getDatabase());
    return resourcesCountDAO.getResourceCount(resId);
}

bool RogalunaLibraryServer::modifyResCount(const QString &resId, int count)
{
    return databaseServer->executeTransaction([&]() {
        Library::ResourcesCountDAO resourcesCountDAO(databaseServer->getDatabase());
        int currentCount = resourcesCountDAO.getResourceCount(resId);
        if (currentCount == 0) {
            // 计数为 0 ，需要插入
            if (count > 0) {
                return resourcesCountDAO.insertResourceCount(resId, count);
            } else {
                return false;
            }
        } else {
            // 计数非 0 ，需要计算
            int countResult = currentCount + count;
            if (countResult > 0) {
                return resourcesCountDAO.modifyResourceCount(resId, countResult);
            } else {
                return resourcesCountDAO.deleteResourceCount(resId);
            }
        }

        return true;
    });


}

bool RogalunaLibraryServer::uplaodLibraryTempFile(const QString tempDirName, const QString &type, const QByteArray &data, const QString &md5)
{
    if (data.size() > maxSingleResSize) {
        // 文件大小超限
        return false;
    } else {
        return storageServer->writeTempFile(tempDirName, type, data, md5);
    }
}

QPair<QByteArray, QString> RogalunaLibraryServer::getLibraryRes(const QString &md5)
{
    // 检查持久存储中是否有对应的资源
    const QString &storageResPath = storageServer->absoluteFilePath(
        root + QDir::separator() +
        resDirName + QDir::separator() +
        md5);

    QDir storageResDir(storageResPath);

    // 在资源目录内，MD5值标识的是一个文件夹，这个文件夹内部有一个以类型名命名的文件
    if (storageResDir.exists()) {
        // 如果持久存储中有这个目录，那么去获取此 md5 值目录下的首个文件。
        QStringList files = storageResDir.entryList(QDir::Files);
        if (!files.isEmpty()) {
            const QString &storageFilePath = storageResPath + QDir::separator() + files.first();
            QFile storageFile(storageFilePath);

            if (storageFile.open(QIODevice::ReadOnly)) {
                QByteArray fileData = storageFile.readAll();
                storageFile.close();

                // 文件名作为类型名
                QString resType = QFileInfo(storageFile).baseName(); // 文件名（不含扩展名）
                return qMakePair(fileData, resType);
            }
        }
    } else {
        // 如果持久存储中没有这个文件，那么去临时文件夹中寻找
        const QString &tempDirPath = storageServer->absoluteFilePath(
            md5, storageServer->temp);

        QDir tempDir(tempDirPath);

        if (tempDir.exists()) {
            // 查找临时文件夹中的文件
            QStringList files = tempDir.entryList(QDir::Files);
            if (!files.isEmpty()) {
                QString tempFilePath = tempDirPath + QDir::separator() + files.first();
                QFile tempFile(tempFilePath);

                if (tempFile.open(QIODevice::ReadOnly)) {
                    QByteArray fileData = tempFile.readAll();
                    tempFile.close();

                    // 文件名作为类型名
                    QString resType = QFileInfo(tempFile).baseName(); // 文件名（不含扩展名）
                    return qMakePair(fileData, resType);
                }
            }
        }
    }

    // 如果未找到资源，返回空结果
    return qMakePair(QByteArray(), QString());
}

bool RogalunaLibraryServer::isResPersExist(const QString &md5)
{
    // 检查持久存储中是否有对应的资源
    const QString &storageResPath = storageServer->absoluteFilePath(
        root + QDir::separator() +
        resDirName + QDir::separator() +
        md5);

    QDir storageResDir(storageResPath);

    if (storageResDir.exists()) {
        QStringList files = storageResDir.entryList(QDir::Files);
        if (!files.isEmpty()) {
            return true;
        }
    }

    return false;
}

bool RogalunaLibraryServer::isResTempExist(const QString &md5)
{
    // 检查临时存储中是否有对应的资源
    const QString &storageResPath = storageServer->absoluteFilePath(
        md5, storageServer->temp);

    QDir storageResDir(storageResPath);

    if (storageResDir.exists()) {
        QStringList files = storageResDir.entryList(QDir::Files);
        if (!files.isEmpty()) {
            return true;
        }
    }

    return false;
}


