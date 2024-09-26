#ifndef CONTENTDAO_H
#define CONTENTDAO_H

#include <BaseDAO.h>

#include <Interface/MusicStation/IContentDAO.h>

namespace MusicStation {

class ROGALUNADATABASESERVER_EXPORT ContentDAO : public BaseDAO, public IContentDAO
{
public:
    explicit ContentDAO(QSqlDatabase& db, const QString& schema = "music_station")
        : BaseDAO(db, schema, "content") {};

    bool insertContent(const QString& contentMd5, qint64 size) override;
};

}

#endif // CONTENTDAO_H
