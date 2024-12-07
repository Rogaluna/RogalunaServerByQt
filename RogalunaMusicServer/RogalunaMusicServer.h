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

        E_OTHERS
    };

    struct FMusicMetadata {

    };

public:
    // ... 其他成员变量和方法

    /**
     * @brief 获取音乐文件路径
     *
     * @param musicMd5 目标音乐的 md5 值，它同时也是文件名称
     * @return 包含音乐文件绝对路径
     */
    QString getMusicFilePath(const QString &musicMd5);

    /**
     * @brief 获取音乐列表
     *
     * @param queryType 查询类型，决定获取音乐的方式
     * @param queryParam 查询参数，根据 queryType 的不同含义不同
     * @param limit 返回结果的最大数量，默认为 10
     * @return 包含音乐元数据的 QVector
     */
    QJsonArray getMusicList(const EMusicQueryType &oper, const QString &query = "");

    /**
     * @brief 上传文件块
     *
     * @param tempDirName 临时目录名称，用于存储分块
     * @param chunkIndex 当前块的索引
     * @param chunkData 当前块的数据
     * @param chunkMd5 当前块的 MD5 校验值，可选
     * @return 如果上传成功返回 true，否则返回 false
     */
    bool uploadChunk(const QString& tempDirName, int chunkIndex, const QByteArray& chunkData, const QString& chunkMd5 = QString());

    /**
     * @brief 合并文件块
     *
     * @param tempDirName 临时目录名称，之前上传的文件块所在的目录
     * @param fileName 合并后的文件名
     * @param totalChunks 文件块的总数量
     * @param userId 上传用户的 ID
     * @param description 音乐描述，可选
     * @return 合并后的文件的 UID，如果失败则返回空字符串
     */
    QString mergeChunks(const QString &tempDirName,
                        const QString &fileName,
                        int totalChunks,
                        int userId,
                        const QString &albumDescription = "",
                        const QString &musicDescription = "",
                        bool bPublished = false);

    /**
     * @brief 保存专辑图片
     *
     * @param musicUid 音乐 uid
     * @param cover 封面图片的内容
     * @return 保存成功与否
     */
    bool saveAlbumCover(const QString &musicUid, const QByteArray &cover);

    /**
     * @brief 获取封面图片
     *
     * @param albumId 专辑 uid
     * @return 图片数据，类型
     */
    QByteArray getCoverImage(const QString id);


    /**
     * @brief 获取音乐元数据
     *
     * @param uids 音乐的 UID 数组
     * @return 如果找到则返回 Metadata 对象，否则返回空值
     */
    QJsonArray getMusicMetadata(const QStringList& uids);

    /**
     * @brief 获取专辑元数据
     *
     * @param ids 专辑的 ID 数组
     * @return 如果找到则返回 Metadata 对象，否则返回空值
     */
    QJsonArray getAlbumsMetadata(const QStringList& ids);

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
    struct AudioMetadata {
        QString artist;
        QString album;
        QString title;
        uint trackNumber;
        QString genre;
        int year;
        QString comment;
        int duration;
        int bitrate;
    };

    AudioMetadata parseAudioFile(QFile &file);
    QByteArray extractAlbumCover(QFile &file, const QString &type);

public:
    QString root;                    ///< 文件存储的根目录。
    QString musicDirName;            ///< 音频文件目录名称。
    QString coverDirName;            ///< 专辑封面目录名称。

private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
};

#endif // ROGALUNAMUSICSERVER_H
