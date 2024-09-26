#ifndef IBOOKCATEGORIESDAO_H
#define IBOOKCATEGORIESDAO_H

#include <QJsonObject>
#include <QString>


namespace Library {

class IBookCategoriesDAO
{
public:
    virtual ~IBookCategoriesDAO() {}

    // 添加标签-书籍映射（单条）
    virtual bool addBookTag(const QString& bookId, int tagId) = 0;

    // 添加标签-书籍映射（多条）
    virtual bool addBookTags(const QString& bookId, const QVector<int>& tagIds) = 0;

    // 删除标签-书籍映射
    virtual bool deleteBookTag(const QString& bookId, int tagId) = 0;

    // 删除与某书籍关联的所有标签映射
    virtual bool deleteAllBookTags(const QString& bookId) = 0;

    // 获取书籍拥有的标签
    virtual QJsonObject getBookTags(const QString& bookId) = 0;
};

}

#endif // IBOOKCATEGORIESDAO_H
