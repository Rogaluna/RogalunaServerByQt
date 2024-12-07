#ifndef ALBUMSDAO_H
#define ALBUMSDAO_H

#include <BaseDAO.h>

#include <Interface/MusicStation/IAlbumsDAO.h>


namespace MusicStation {

class ROGALUNADATABASESERVER_EXPORT AlbumsDAO: public BaseDAO, public IAlbumsDAO
{
public:
    explicit AlbumsDAO(QSqlDatabase& db, const QString& schema = "music_station")
        : BaseDAO(db, schema, "albums") {}

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
    QString insertAlbum(const QString &albumName,
                        const QString &artist,
                        int releaseYear,
                        const QString &genre,
                        const QString &description) override;

    /**
     * @brief 根据专辑 ID 获取专辑信息。
     *
     * @param ids 要查询的专辑 ID 数组。
     * @return 查询到的专辑信息。
     */
    QJsonArray getAlbumsById(const QStringList &ids) override;
};

}

#endif // ALBUMSDAO_H
