#ifndef USERCOLLECTIONSDAO_H
#define USERCOLLECTIONSDAO_H

#include <BaseDAO.h>

#include <Interface/Library/IUserCollectionsDAO.h>

namespace Library {

class ROGALUNADATABASESERVER_EXPORT UserCollectionsDAO : public BaseDAO, public IUserCollectionsDAO
{
public:
    explicit UserCollectionsDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "user_collections") {}

public:
    // 获取用户收藏的书籍 id
    QStringList getBooksByUserCollection(const QString &userId) override;
};

}

#endif // USERCOLLECTIONSDAO_H
