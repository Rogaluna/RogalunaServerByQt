#ifndef READPROGRESSDAO_H
#define READPROGRESSDAO_H

#include <BaseDAO.h>

#include <Interface/Library/IReadProgressDAO.h>

namespace Library {

class ROGALUNADATABASESERVER_EXPORT ReadProgressDAO : public BaseDAO, public IReadProgressDAO
{
public:
    explicit ReadProgressDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "reading_progress") {}

    virtual bool updateProgress(int userId, const QString& bookId, int chapterNumber, int progress) override;

    virtual QJsonObject getProgress(int userId, const QString& bookId) override;

    virtual bool deleteProgress(int userId, const QString& bookId) override;
};

}

#endif // READPROGRESSDAO_H
