#include "RogalunaMusicServer.h"

RogalunaMusicServer::RogalunaMusicServer(
    RogalunaStorageServer* storageServer,
    RogalunaDatabaseServer* databaseServer,
    const QString &root,
    const QString &musicDirName,
    const QString &coverDirName)
    : storageServer(storageServer)
    , databaseServer(databaseServer)
    , root(root)
    , musicDirName(musicDirName)
    , coverDirName(coverDirName)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(".");  // 创建根文件夹
    }

    QString musicDirPath = rootPath + QDir::separator() + musicDirName;
    QDir musicDir(musicDirPath);
    if (!musicDir.exists()) {
        musicDir.mkpath("."); // 构造音乐目录的路径
    }

    QString coverDirPath = rootPath + QDir::separator() + coverDirName;
    QDir coverDir(coverDirPath);
    if (!coverDir.exists()) {
        coverDir.mkpath("."); // 构造封面目录的路径
    }
}

// std::optional<QVector<MusicStation::FFileMetadata>> RogalunaMusicServer::getMusicList(const QString &query, const EMusicQueryType &Operator)
// {
//     Q_UNUSED(query)

//     std::optional<QVector<MusicStation::FFileMetadata>> result;

//     switch(Operator) {
//     case EMusicQueryType::E_RANDOM:
//         break;
//     case EMusicQueryType::E_PRIVATE:
//         break;
//     case EMusicQueryType::E_BYALBUM:
//         break;
//     case EMusicQueryType::E_BYUSERID:
//         break;
//     default:
//         break;
//     }

//     return result;
// }
