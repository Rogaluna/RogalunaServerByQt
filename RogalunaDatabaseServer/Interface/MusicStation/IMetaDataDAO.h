#ifndef IMETADATADAO_H
#define IMETADATADAO_H

#include <QString>
#include <QDateTime>

namespace MusicStation {

struct FFileMetadata {
    QString uid;
    int userId;
    QString contentMd5;
    QString fileName;
    QString description;
    int albumId;
    int bitrate;
    bool bPublished;
    QDateTime createdAt;
    QDateTime updatedAt;
};

class IMetadataDAO
{
public:
    virtual ~IMetadataDAO() {}

    /**
     * @brief 插入一个新的元数据记录。
     *
     * @param userId 用户的ID。
     * @param fileName 文件名。
     * @param contentMd5 内容的MD5值。
     * @param description 文件描述。
     * @param albumId 专辑ID。
     * @param bitrate 比特率（kbps）。
     * @param bPublished 是否需要发布。
     * @return 新插入的元数据的UID。
     */
    virtual QString insertMetadata(int userId,
                                   const QString &fileName,
                                   const QString &contentMd5,
                                   const QString &description,
                                   const QString &albumId,
                                   int bitrate,
                                   bool bPublished) = 0;

    /**
     * @brief 更新元数据记录。
     *
     * @param metadata 要更新的元数据。
     * @return 如果更新成功返回 true，否则返回 false。
     */
    // virtual bool updateMetadata(const FFileMetadata& metadata) = 0;

    /**
     * @brief 删除元数据记录。
     *
     * @param uid 要删除的元数据的 UID。
     * @return 如果删除成功返回 true，否则返回 false。
     */
    // virtual bool deleteMetadata(const QString& uid) = 0;

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
    // virtual std::optional<QVector<FFileMetadata>> getMetadataByUserId(int user_id) = 0;
};

}

#endif // IMETADATADAO_H
