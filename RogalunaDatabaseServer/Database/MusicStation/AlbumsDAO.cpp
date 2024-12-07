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

    // 执行插入
    if (insertQuery.exec()) {
        return albumId;
    }

    // 插入失败，记录错误并返回空字符串
    qWarning() << "Failed to insert new album for albumName:" << albumName << insertQuery.lastError().text();

    return QString();
}

QJsonArray MusicStation::AlbumsDAO::getAlbumsById(const QStringList &ids)
{
    if (ids.isEmpty()) {
        return QJsonArray();  // 如果 uid 列表为空，直接返回空数组
    }

    QSqlQuery query(database);
    // 构建占位符字符串
    QStringList quotedUids;
    for (const QString &uid : ids) {
        quotedUids << QString("'%1'").arg(uid);  // 给每个 uid 加上单引号
    }
    QString placeholders = quotedUids.join(", ");  // 用逗号连接带引号的 uid
    QString sql = QString("SELECT * FROM %1 WHERE album_id IN (%2)").arg(fullTableName(), placeholders);
    query.prepare(sql);
    // 绑定每个 uid 到占位符
    for (int i = 0; i < ids.size(); ++i) {
        query.bindValue(i, ids.at(i));
    }

    if (!query.exec()) {
        qWarning() << "Failed to execute query:" << query.lastError().text();
        return QJsonArray();  // 查询失败，返回空数组
    }

    QJsonArray metadataArray;
    if (query.next()) {
        QJsonObject metadata;
        metadata["album_id"] = query.value("album_id").toString();
        metadata["album_name"] = query.value("album_name").toString();
        metadata["artist"] = query.value("artist").toString();
        metadata["release_year"] = query.value("release_year").toInt();
        metadata["genre"] = query.value("genre").toString();
        metadata["description"] = query.value("description").toString();
        metadata["created_at"] = query.value("created_at").toString();
        metadata["updated_at"] = query.value("updated_at").toString();

        metadataArray.append(metadata);  // 将 JSON 对象添加到数组中
    }

    return metadataArray;
}
