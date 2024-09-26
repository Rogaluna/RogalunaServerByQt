#ifndef BOOKSDAO_H
#define BOOKSDAO_H

#include <BaseDAO.h>
#include <Interface/Library/IBooksDAO.h>


namespace Library {

class ROGALUNADATABASESERVER_EXPORT BooksDAO : public BaseDAO, public IBooksDAO
{
public:
    explicit BooksDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "books") {}

    // 实现 IBooksDAO 的接口

    // 随机获取书籍
    QJsonArray getBooksByRandom(int rangeSize) override;

    // 获取用户撰写书籍
    QJsonArray getBooksByUserWrite(const QString &userId, int rangeSize) override;

    // 通过书籍 id 获取书籍列表
    QJsonArray getBooksByIds(QStringList bookIds) override;

    // 获取特定书籍的详细信息
    QJsonObject getBookDetails(const QString& bookId) override;

    // 添加新书籍，返回成功插入的 bookId（UUID），若失败返回空字符串
    QString addBook(const QString & userId, const QString& bookName,
                            const QString& bookDesc) override;

    // 修改书籍信息
    bool updateBook(const QString& bookId, const QString& bookName,
                            const QString& bookDesc) override;

    // 删除书籍
    bool deleteBook(const QString& bookId) override;
};

}

#endif // BOOKSDAO_H
