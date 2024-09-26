#ifndef BOOKCATEGORIESDAO_H
#define BOOKCATEGORIESDAO_H

#include <BaseDAO.h>

#include <Interface/Library/IBookCategoriesDAO.h>

namespace Library {

class ROGALUNADATABASESERVER_EXPORT BookCategoriesDAO : public BaseDAO, public IBookCategoriesDAO
{
public:
    explicit BookCategoriesDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "book_categories") {}

public:
    // 添加单条标签映射
    bool addBookTag(const QString& bookId, int tagId) override;

    // 批量添加多条标签映射
    bool addBookTags(const QString& bookId, const QVector<int>& tagIds) override;

    // 删除单条标签映射
    bool deleteBookTag(const QString& bookId, int tagId) override;

    // 删除指定书籍的所有标签映射
    bool deleteAllBookTags(const QString& bookId) override;

    // 获取书籍拥有的标签
    QJsonObject getBookTags(const QString& bookId) override;
};

}



#endif // BOOKCATEGORIESDAO_H
