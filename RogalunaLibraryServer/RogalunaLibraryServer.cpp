#include "RogalunaLibraryServer.h"

#include <QJsonArray>
#include <QJsonObject>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>

#include <Database/Library/BookCategoriesDAO.h>
#include <Database/Library/BooksDAO.h>
#include <Database/Library/CategoriesDAO.h>
#include <Database/Library/ChaptersDAO.h>
#include <Database/Library/ReadProgressDAO.h>
#include <Database/Library/UserCollectionsDAO.h>

RogalunaLibraryServer::RogalunaLibraryServer(
    RogalunaStorageServer *storageServer,
    RogalunaDatabaseServer *databaseServer,
    const QString &root,
    const QString &bookDirName,
    const QString &coverDirName,
    int maxRangeSize,
    int maxSingleResSize,
    int categoryRootId)
    : root(root)
    , bookDirName(bookDirName)
    , coverDirName(coverDirName)
    , storageServer(storageServer)
    , databaseServer(databaseServer)
    , categoryRootId(categoryRootId)
    , maxRangeSize(maxRangeSize)
    , maxSingleResSize(maxSingleResSize)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(QDir::separator());  // 创建根文件夹
    }

    QString bookDirPath = rootPath + QDir::separator() + bookDirName;
    QDir book(bookDirPath);
    if (!book.exists()) {
        book.mkpath(QDir::separator()); // 构造书籍目录的路径
    }

    QString coverDirPath = rootPath + QDir::separator() + coverDirName;
    QDir coverDir(coverDirPath);
    if (!coverDir.exists()) {
        coverDir.mkpath(QDir::separator()); // 构造封面目录的路径
    }
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
                    bookDirName + QDir::separator() +
                    bookId + QDir::separator() +
                    fileName + QDir::separator() +
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
        fileName + QDir::separator() +
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
                    fileName + QDir::separator() +
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

bool RogalunaLibraryServer::uplaodChapterResource(const QString &bookId, const QString &chapterName, const QString resName, const QString &type, const QByteArray &data, const QString &md5)
{
    if (data.size() > maxSingleResSize) {
        // 文件大小超限
        return false;
    } else {
        // 根据章节序号获取对应的名称
        const QString &targetPath = storageServer->absoluteFilePath(
            root + QDir::separator() +
            bookDirName + QDir::separator() +
            bookId + QDir::separator() +
            chapterName + QDir::separator() +
            resName + QDir::separator() +
            type);

        return storageServer->writeFile(targetPath, data, md5);
    }
}

QPair<QByteArray, QString> RogalunaLibraryServer::getChapterResource(const QString &bookId, const QString &chapterName, const QString &resName)
{
    // 检查持久存储中是否有对应的资源
    const QString &storageResPath = storageServer->absoluteFilePath(
        root + QDir::separator() +
        bookDirName + QDir::separator() +
        bookId + QDir::separator() +
        chapterName + QDir::separator() +
        resName);

    QDir storageResDir(storageResPath);
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
    }
    // 如果未找到资源，返回空结果
    return qMakePair(QByteArray(), QString());
}

bool RogalunaLibraryServer::deleteChapterResource(const QString &bookId, const QString &chapterName, const QString &resName)
{
    // 检查持久存储中是否有对应的资源
    const QString &storageResPath = storageServer->absoluteFilePath(
        root + QDir::separator() +
        bookDirName + QDir::separator() +
        bookId + QDir::separator() +
        chapterName + QDir::separator() +
        resName);

    QDir storageResDir(storageResPath);
    if (storageResDir.exists()) {
        // 如果有这个资源，则进行删除
        return storageServer->deleteFile(storageResDir);
    }

    return false;
}

bool RogalunaLibraryServer::uplaodBookCover(const QString &bookId, const QString &resType, const QByteArray &data, const QString &md5)
{
    if (data.size() > maxSingleResSize) {
        // 文件大小超限
        return false;
    } else {
        // 将原先的封面删掉
        const QString &storageResPath = storageServer->absoluteFilePath(
            root + QDir::separator() +
            coverDirName + QDir::separator() +
            bookId);
        QDir storageResDir(storageResPath);
        if (storageResDir.exists()) {
            storageServer->deleteFile(storageResDir);
        }

        const QString &targetPath = storageServer->absoluteFilePath(
            root + QDir::separator() +
            coverDirName + QDir::separator() +
            bookId + QDir::separator() +
            resType);
        return storageServer->writeFile(targetPath, data, md5);
    }
}

QPair<QByteArray, QString> RogalunaLibraryServer::getBookCover(const QString &bookId)
{
    // 检查持久存储中是否有对应的资源
    const QString &storageResPath = storageServer->absoluteFilePath(
        root + QDir::separator() +
        coverDirName + QDir::separator() +
        bookId);

    QDir storageResDir(storageResPath);
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
    }

    return qMakePair(QByteArray(), "");
}

QStringList RogalunaLibraryServer::findRemovedResources(const QString &oldHtmlContent, const QString &newHtmlContent)
{
    QStringList oldResources = extractHtmlResRef(oldHtmlContent);
    QStringList newResources = extractHtmlResRef(newHtmlContent);

    // 转换为 QSet 进行比较
    QSet<QString> oldSet = QSet<QString>(oldResources.begin(), oldResources.end());
    QSet<QString> newSet = QSet<QString>(newResources.begin(), newResources.end());

    // 计算差集：旧内容中存在但新内容中不存在的资源
    QSet<QString> removedSet = oldSet - newSet;

    return QStringList(removedSet.begin(), removedSet.end());
}

QStringList RogalunaLibraryServer::extractHtmlResRef(const QString &htmlContent)
{
    QStringList srcList;  // 用于存储提取的 src 内容

    // 将 QString 转换为 const char*
    QByteArray htmlData = htmlContent.toUtf8();
    const char *html = htmlData.constData();

    // 1. 解析HTML文档
    htmlDocPtr doc = htmlReadMemory(html, htmlData.size(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) {
        qWarning() << "Failed to parse HTML document!";
        return srcList;
    }

    // 2. 创建XPath上下文
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx) {
        qWarning() << "Failed to create XPath context!";
        xmlFreeDoc(doc);
        return srcList;
    }

    // 3. 提取 img 和 audio 标签的 src 属性
    const char *xpathExprs[] = {
        "//img/@src",     // XPath表达式：获取img标签的src属性
        "//audio/@src"    // XPath表达式：获取audio标签的src属性
    };

    for (const char *expr : xpathExprs) {
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar *)expr, xpathCtx);
        if (!xpathObj) {
            qWarning() << "Failed to evaluate XPath expression!";
            continue;
        }

        xmlNodeSetPtr nodes = xpathObj->nodesetval;
        if (!nodes) continue;

        // 4. 遍历结果，将 src 内容添加到 QStringList
        for (int i = 0; i < nodes->nodeNr; ++i) {
            xmlNodePtr node = nodes->nodeTab[i];
            if (node->type == XML_ATTRIBUTE_NODE && node->children) {
                srcList.append(QString::fromUtf8((const char *)node->children->content));
            }
        }

        xmlXPathFreeObject(xpathObj);
    }

    // 5. 清理资源
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);

    return srcList;  // 返回提取的 src 列表
}


