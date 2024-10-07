#ifndef IMETADATADAO_H
#define IMETADATADAO_H

#include <QString>
#include <QDateTime>

struct FFileMetadata {
    int uid;
    int user_id;
    QString content_md5;
    QString filename;
    QString description;
    QString artist;
    QString album;
    int bitrate;
    QString genre;
    int release_year;
    bool is_published;
    QDateTime created_at;
    QDateTime updated_at;
};

class IMetadataDAO
{
public:
    virtual ~IMetadataDAO() {}

    /**
     * @brief 插入一个新的元数据记录。
     *
     * @param metadata 要插入的元数据。
     * @return 新插入的元数据的 UID。
     */
    virtual QString insertMetadata(const FFileMetadata& metadata) = 0;

    /**
     * @brief 更新元数据记录。
     *
     * @param metadata 要更新的元数据。
     * @return 如果更新成功返回 true，否则返回 false。
     */
    virtual bool updateMetadata(const FFileMetadata& metadata) = 0;

    /**
     * @brief 删除元数据记录。
     *
     * @param uid 要删除的元数据的 UID。
     * @return 如果删除成功返回 true，否则返回 false。
     */
    virtual bool deleteMetadata(const QString& uid) = 0;

    /**
     * @brief 根据 UID 获取元数据。
     *
     * @param uid 要获取的元数据的 UID。
     * @return 如果找到则返回元数据，否则返回 std::nullopt。
     */
    virtual std::optional<FFileMetadata> getMetadataByUid(const QString& uid) = 0;

    /**
     * @brief 获取随机元数据记录。
     *
     * @return 包含随机元数据记录的向量。
     */
    virtual std::optional<QVector<FFileMetadata>> getRandomMetadata() = 0;

    /**
     * @brief 根据用户 ID 获取元数据记录。
     *
     * @param user_id 用户的 ID。
     * @return 包含该用户所有元数据记录的向量。
     */
    virtual std::optional<QVector<FFileMetadata>> getMetadataByUserId(int user_id) = 0;
};

#endif // IMETADATADAO_H
