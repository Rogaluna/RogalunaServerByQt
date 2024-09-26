#ifndef IBOOKSDAO_H
#define IBOOKSDAO_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>

namespace Library {

enum class FGetBookOpt : uint {
    E_NORMAL,
    E_USER_COLLECTION,
    E_USER_WRITE
};


class IBooksDAO
{
public:
    virtual ~IBooksDAO() {}

    // 随机获取书籍
    virtual QJsonArray getBooksByRandom(int rangeSize) = 0;

    // 获取用户撰写书籍
    virtual QJsonArray getBooksByUserWrite(const QString &userId, int rangeSize) = 0;

    // 通过书籍 id 获取书籍列表
    virtual QJsonArray getBooksByIds(QStringList bookIds) = 0;

    // 获取特定书籍的详细信息
    virtual QJsonObject getBookDetails(const QString& bookId) = 0;

    // 添加新书籍，返回成功插入的 bookId（UUID），若失败返回空字符串
    virtual QString addBook(const QString & userId, const QString& bookName,
                            const QString& bookDesc) = 0;

    // 修改书籍信息
    virtual bool updateBook(const QString& bookId, const QString& bookName,
                            const QString& bookDesc) = 0;

    // 删除书籍
    virtual bool deleteBook(const QString& bookId) = 0;
};

}

#endif // IBOOKSDAO_H
