#ifndef ROGALUNAMUSICSERVER_H
#define ROGALUNAMUSICSERVER_H

#include "RogalunaMusicServer_global.h"

#include <QObject>

#include <RogalunaDatabaseServer.h>
#include <RogalunaStorageServer.h>

class ROGALUNAMUSICSERVER_EXPORT RogalunaMusicServer
{
public:
    RogalunaMusicServer(
        RogalunaStorageServer* storageServer,
        RogalunaDatabaseServer* databaseServer,
        const QString &root,
        const QString &musicDirName,
        const QString &coverDirName);

public:
    enum class EMusicQueryType : char  {
        E_RANDOM,
        E_PRIVATE,
        E_BYALBUM,
        E_BYUSERID,
    };

public:
    // ... 其他成员变量和方法

    /**
     * @brief 获取音乐列表
     *
     * @param queryType 查询类型，决定获取音乐的方式
     * @param queryParam 查询参数，根据 queryType 的不同含义不同
     * @param limit 返回结果的最大数量，默认为 10
     * @return 包含音乐元数据的 QVector
     */
    // std::optional<QVector<MusicStation::FFileMetadata>> getMusicList(const QString &query, const EMusicQueryType &Operator);

    /**
     * @brief 上传文件块
     *
     * @param tempDirName 临时目录名称，用于存储分块
     * @param chunkIndex 当前块的索引
     * @param chunkData 当前块的数据
     * @param chunkMd5 当前块的 MD5 校验值，可选
     * @return 如果上传成功返回 true，否则返回 false
     */
    // bool uploadChunk(const QString& tempDirName, int chunkIndex, const QByteArray& chunkData, const QString& chunkMd5 = QString());

    /**
     * @brief 合并文件块
     *
     * @param tempDirName 临时目录名称，之前上传的文件块所在的目录
     * @param fileName 合并后的文件名
     * @param totalChunks 文件块的总数量
     * @param userId 上传用户的 ID
     * @param parentUid 父目录的 UID，可选
     * @return 合并后的文件的 UID，如果失败则返回空字符串
     */
    // QString mergeChunks(const QString& tempDirName, const QString& fileName, int totalChunks, int userId, const QString& parentUid = QString());

    /**
     * @brief 获取音乐元数据
     *
     * @param uid 音乐的 UID
     * @return 如果找到则返回 Metadata 对象，否则返回 std::nullopt
     */
    // std::optional<FFileMetadata> getMusicMetadata(const QString& uid);

    /**
     * @brief 处理音乐文件的下载请求
     *
     * @param uid 音乐的 UID
     * @param request 请求对象，包含 Range 等信息
     * @param response 响应对象，用于写入数据
     * @return 如果成功处理返回 true，否则返回 false
     */
    // bool downloadMusic(const QString& uid, const QHttpRequest& request, QHttpResponse& response);

    /**
     * @brief 修改音乐元数据
     *
     * @param uid 音乐的 UID
     * @param newMetadata 包含要更新的字段的 Metadata 对象
     * @return 如果更新成功返回 true，否则返回 false
     */
    // bool updateMusicMetadata(const QString& uid, const Metadata& newMetadata);

    /**
     * @brief 更新音乐的专辑图片
     *
     * @param uid 音乐的 UID
     * @param imageData 图片的二进制数据
     * @return 如果更新成功返回 true，否则返回 false
     */
    // bool updateAlbumCover(const QString& uid, const QByteArray& imageData);

    /**
     * @brief 删除音乐
     *
     * @param uid 音乐的 UID
     * @return 如果删除成功返回 true，否则返回 false
     */
    // bool deleteMusic(const QString& uid);

private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
    QString root;
    QString musicDirName;
    QString coverDirName;
};

#endif // ROGALUNAMUSICSERVER_H
