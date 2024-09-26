#include "ContentDAO.h"

namespace MusicStation {

bool MusicStation::ContentDAO::insertContent(const QString &contentMd5, qint64 size)
{
    QSqlQuery query(database);
    QString sql = QString("INSERT INTO %1 (content_md5, size, created_at) "
                          "VALUES (:content_md5, :size, NOW())")
                      .arg(fullTableName());
    query.prepare(sql);
    query.bindValue(":content_md5", contentMd5);
    query.bindValue(":size", size);
    return executeQuery(query);
}

}
