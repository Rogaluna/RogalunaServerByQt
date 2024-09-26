#ifndef IMETADATADAO_H
#define IMETADATADAO_H

#include <QDateTime>
#include <QString>

namespace CloudDrive {

struct FFileMetadata {
    QString uid;
    int userId;
    QString fileName;
    QString contentMd5;
    QString parentUid;
    QDateTime uploadedAt;
    bool isDirectory;
    int version;
    QDateTime createdAt;
    QDateTime updatedAt;
};

class IMetadataDAO
{
public:
    virtual ~IMetadataDAO() {}

    /**
     * @brief 插入一个新的文件或文件夹元数据。
     *
     * @param userId 文件或文件夹所属用户的 ID。
     * @param fileName 文件或文件夹的名称。
     * @param contentMd5 文件内容的 MD5 哈希值。对于文件夹应为空。
     * @param isDirectory 布尔值，指示该元数据是否指代文件夹。
     * @param parentUid 父文件夹的 UID。如果没有指定父文件夹，默认为空字符串（即根目录）。
     * @param version 文件或文件夹的版本号，默认为 1。
     * @return 新插入的文件或文件夹的 UID。
     */
    virtual QString insertMetadata(int userId, const QString& fileName, const QString &contentMd5, bool isDirectory, const QString& parentUid = QString(), int version = 1) = 0;

    /**
     * @brief 删除文件或文件夹的元数据。
     *
     * @param uid 要删除的文件或文件夹的 UID。
     * @return 如果删除成功返回 true，否则返回 false。
     */
    virtual bool deleteMetadata(const QString& uid) = 0;

    /**
     * @brief 获取用户的根目录的 UID。
     *
     * @param userId 用户的 ID。
     * @return 返回用户根目录的 UID。
     */
    virtual QString getUserRootDirUid(int userId) = 0;

    /**
     * @brief 插入一个新的文件夹。
     *
     * @param userId 文件夹所属用户的 ID。
     * @param parentUid 父文件夹的 UID。
     * @param folderName 文件夹名称。
     * @return 新插入的文件夹的 UID。
     */
    virtual QString insertFolder(int userId, const QString &parentUid, const QString &folderName) = 0;

    /**
     * @brief 获取指定 uid 对象的展平路径。
     *
     * @param 指定对象的 UID。
     * @return 返回完整的展平路径，以字符串形式表示。
     */
    virtual QString getPath(const QString& uid) = 0;

    /**
     * @brief 根据展平路径和用户ID获取最终对象的 FFileMetadata。
     *
     * @param path 以 '/' 分割的展平路径（如 "root/test"）。
     * @param userId 指定用户的 ID。
     * @return 返回路径对应的最终对象的 FFileMetadata。如果路径无效或不属于该用户则返回 std::nullopt。
     */
    virtual std::optional<FFileMetadata> getMetadataFromPath(const QString& path, int userId) = 0;

    /**
     * @brief 获取用户拥有的文件或文件夹。
     *
     * @param userId 用户的 ID。
     * @return 包含文件或文件夹元数据的可选 QVector 数组，如果用户没有文件或文件夹，则为空。
     */
    virtual std::optional<QVector<FFileMetadata>> getMetadataByUserId(int userId) = 0;

    /**
     * @brief 根据 UID 获取文件或文件夹的元数据（只返回一个元素的数组）。
     *
     * @param uid 文件或文件夹的 UID。
     * @return 包含该 UID 的文件或文件夹元数据的可选 QVector 数组，如果没有找到则为空。
     */
    virtual std::optional<QVector<FFileMetadata>> getMetadataByUid(const QString &uid) = 0;

    /**
     * @brief 获取特定文件夹 UID 下的文件或文件夹。
     *
     * @param folderUid 文件夹的 UID。
     * @return 包含该文件夹下的文件或文件夹元数据的可选 QVector 数组，如果文件夹为空则返回空。
     */
    virtual std::optional<QVector<FFileMetadata>> getMetadataUnderFolder(const QString &folderUid) = 0;

    /**
     * @brief 更新文件/文件夹名。
     *
     * @param uid 要进行更新的文件或文件夹的 UID。
     * @param name 覆写的名称。
     * @return 如果删除成功返回 true，否则返回 false。
     */
    virtual bool updateFileName(const QString& uid, const QString &name) = 0;

    /**
     * @brief 获取字段相同值的计数。
     *
     * @param field 要进行查找的字段名称。
     * @param value 要进行查找字段的值。
     * @return 返回记录计数。
     */
    virtual int getRefCount(const QString& field, const QString &value) = 0;

};

}

#endif // IMETADATADAO_H
