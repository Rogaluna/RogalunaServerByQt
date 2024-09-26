#ifndef ROGALUNACLOUDDRIVESERVER_H
#define ROGALUNACLOUDDRIVESERVER_H

#include "RogalunaCloudDriveServer_global.h"

#include <RogalunaStorageServer.h>
#include <RogalunaDatabaseServer.h>

#include <Interface/CloudDrive/IMetadataDAO.h>

class ROGALUNACLOUDDRIVESERVER_EXPORT RogalunaCloudDriveServer
{
public:
    RogalunaCloudDriveServer(RogalunaStorageServer* storageServer, RogalunaDatabaseServer* databaseServer, const QString &root);

public:
    enum class EGetFileOpterator : char {
        E_UID,
        E_USERID,
        E_FOLDER,
    };

public:
    // 上传文件块
    bool uploadChunk(const QString tempDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5 = "");

    // 插入文件夹，返回文件的 uid
    QString createFolder(int userId, const QString &parentUid, const QString &folderName);

    // 合并文件块，返回文件的 uid
    QString mergeChunks(const QString &tempDirName, const QString &fileName, int totalChunks, int userId, const QString &parentUid = QString());

    // 下载文件，通过 MD5 获取文件数据
    /* 注：这个函数实际上没有被使用过，预计会在获取文件的地方调用它，之后的修改可以酌情参考 */
    FileReadResult downloadFile(const QString &contentMd5);

    // 删除文件，通过 uid 删除文件和数据库记录，如果传入的 userId 是空值，则不进行权限检查
    bool deleteFile(const QString &uid, const QString& userId = "");

    // 获取某个用户拥有的文件（选项：特定uid、特定userId、特定文件夹uid）
    std::optional<QVector<CloudDrive::FFileMetadata>> getFiles(const QString &query, const EGetFileOpterator &Operator);

    // 获取特定 uid 的路径全称
    QString getPath(const QString &uid);

    // 获取用户的根目录 uid
    QString getUserRootDirUid(int userId);

    // 获取一个 uid 对象的父对象
    std::optional<CloudDrive::FFileMetadata> getParent(const QString &uid);

    // 根据路径获取 uid 对象
    std::optional<CloudDrive::FFileMetadata> getMetadataFromPath(const QString& path, int userId);

    // 获取目标文件，得到文件数据等信息
    bool getTargetFile(const QString &targetMd5, bool isRangeRequest, QByteArray& fileData, qint64 &startPos, qint64 &endPos, qint64 &fileSize);

    // 重命名文件/文件夹名称
    bool renameFile(const QString &uid, const QString &newName);

public:
    QString root;                    ///< 文件存储的根目录。

private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
};

#endif // ROGALUNACLOUDDRIVESERVER_H
