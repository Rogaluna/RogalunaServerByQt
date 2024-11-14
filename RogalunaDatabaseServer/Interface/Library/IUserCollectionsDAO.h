#ifndef IUSERCOLLECTIONSDAO_H
#define IUSERCOLLECTIONSDAO_H

#include <QString>


namespace Library {

class IUserCollectionsDAO
{
public:
    virtual ~IUserCollectionsDAO() {}

    // 获取用户收藏的书籍 id
    virtual QStringList getBooksByUserCollection(const QString &userId) = 0;
};

}

#endif // IUSERCOLLECTIONSDAO_H
