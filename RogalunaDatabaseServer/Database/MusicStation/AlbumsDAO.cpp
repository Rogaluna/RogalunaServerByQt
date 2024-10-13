#include "AlbumsDAO.h"

#include <QCryptographicHash>

QString MusicStation::AlbumsDAO::insertAlbum(const QString &albumName,
                                             const QString &artist,
                                             int releaseYear,
                                             const QString &genre,
                                             const QString &description)
{
    // 构建专辑 ID
    QCryptographicHash md5Hash(QCryptographicHash::Md5);
    md5Hash.addData(albumName.toUtf8());
    md5Hash.addData(artist.toUtf8());
    md5Hash.addData(QString::number(releaseYear).toUtf8());
    md5Hash.addData(genre.toUtf8());
    md5Hash.addData(description.toUtf8());
    QString albumId = md5Hash.result().toHex();

    QSqlQuery insertQuery(database);
    QString sql = QString("INSERT INTO %1 (album_id, album_name, artist, release_year, genre, description) "
                          "VALUES (:album_id, :album_name, :artist, :release_year, :genre, :description)")
                      .arg(fullTableName());
    insertQuery.prepare(sql);
    insertQuery.bindValue(":album_id", albumId);
    insertQuery.bindValue(":album_name", albumName);
    insertQuery.bindValue(":artist", artist);
    insertQuery.bindValue(":release_year", releaseYear);
    insertQuery.bindValue(":genre", genre);
    insertQuery.bindValue(":description", description);

    // 执行插入并获取新生成的 uid
    if (insertQuery.exec() && insertQuery.next()) {
        return insertQuery.value("uid").toString();  // 返回数据库生成的 uid
    }

    // 插入失败，记录错误并返回空字符串
    qWarning() << "Failed to insert new album for albumName:" << albumName << insertQuery.lastError().text();

    return QString();
}
