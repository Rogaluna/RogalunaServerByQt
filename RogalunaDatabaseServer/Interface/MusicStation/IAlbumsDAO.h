#ifndef IALBUMSDAO_H
#define IALBUMSDAO_H

#include <QJsonObject>
#include <QJsonArray>
#include <QString>

namespace MusicStation {

class Album
{
public:
    QString albumId;
    QString title;
    QString artist;
    QString genre;
    int releaseYear;
    // 其他专辑相关的属性可以在这里添加
};

class IAlbumsDAO
{
public:
    virtual ~IAlbumsDAO() {}

public:
    /**
     * @brief 更新专辑信息。
     *
     * @param albumName 新的专辑名称。
     * @param artist 新的艺术家名称。
     * @param releaseYear 新的发行年份。
     * @param genre 新的音乐流派。
     * @return 插入的 id 序号。
     */
    virtual QString insertAlbum(const QString &albumName,
                                const QString &artist,
                                int releaseYear,
                                const QString &genre,
                                const QString &description) = 0;

    /**
     * @brief 根据专辑 ID 获取专辑信息。
     *
     * @param ids 要查询的专辑 ID 数组。
     * @return 查询到的专辑信息。
     */
    virtual QJsonArray getAlbumsById(const QStringList &ids) = 0;

    /**
     * @brief 更新专辑信息。
     *
     * @param album 要更新的专辑信息（包含ID）。
     * @return 是否更新成功。
     */
    // virtual bool updateAlbum(const Album &album) = 0;

    /**
     * @brief 删除一条专辑记录。
     *
     * @param albumId 要删除的专辑ID。
     * @return 是否删除成功。
     */
    // virtual bool deleteAlbum(const QString &albumId) = 0;

    /**
     * @brief 获取所有专辑的列表。
     *
     * @return 专辑列表。
     */
    // virtual QList<Album> getAllAlbums() = 0;
};

}

#endif // IALBUMSDAO_H
