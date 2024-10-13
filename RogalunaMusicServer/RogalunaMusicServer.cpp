#include "RogalunaMusicServer.h"

#include <Database/MusicStation/AlbumsDAO.h>
#include <Database/MusicStation/ContentDAO.h>
#include <Database/MusicStation/MetadataDAO.h>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include <Cover/AlbumCoverHandler.h>
#include <Cover/FLACAlbumCoverHandler.h>
#include <Cover/MP3AlbumCoverHandler.h>

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

bool RogalunaMusicServer::uploadChunk(const QString &tempDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5)
{
    // 将块数据写入临时文件夹
    return storageServer->writeTempFile(tempDirName, chunkIndex, chunkData, chunkMd5);
}

QString RogalunaMusicServer::mergeChunks(
    const QString &tempDirName,
    const QString &fileName,
    int totalChunks,
    int userId,
    const QString &albumDescription,
    const QString &musicDescription,
    bool bPublished)
{
    // 需要进行合并的文件夹名称即是文件的 MD5 值
    const QString& targetMd5 = tempDirName;

    // 目标文件存储路径
    QString targetName = root + QDir::separator() + targetMd5;

    // 合并临时文件夹中的文件块
    if (!storageServer->mergeTempFile(tempDirName, totalChunks, root, targetMd5)) {
        qWarning() << "Failed to merge chunks";
        return QString();
    }

    QFile mergedFile(storageServer->absoluteFilePath(targetName));
    if (!mergedFile.exists()) {
        qWarning() << "Merged file not found";
        return QString();
    }

    // 获取音频文件内存储的元数据
    AudioMetadata audioMetadata = parseAudioFile(mergedFile);

    MusicStation::AlbumsDAO albumsDao(databaseServer->getDatabase());
    MusicStation::MetadataDAO metadataDao(databaseServer->getDatabase());
    MusicStation::ContentDAO contentDao(databaseServer->getDatabase());

    // 插入专辑信息记录，获取生成的 id 值
    QString albumId = albumsDao.insertAlbum(
        audioMetadata.album,
        audioMetadata.artist,
        audioMetadata.year,
        audioMetadata.genre,
        albumDescription);
    if (albumId.isEmpty()) {
        qWarning() << "Failed to insert file album info into database";
        return QString();
    }

    // 插入文件内容记录
    if (!contentDao.insertContent(targetMd5, mergedFile.size())) {
        qWarning() << "Failed to insert file content into database";
        return QString();
    }

    // 插入文件元数据记录, uuid 由数据库自动生成
    QString uuid = metadataDao.insertMetadata(
        userId,
        fileName,
        targetMd5,
        musicDescription,
        albumId,
        audioMetadata.bitrate,
        bPublished);
    if (uuid.isEmpty()) {
        qWarning() << "Failed to insert file metadata into database";
        return QString();
    }

    // 在数据库完整写入后，导入专辑封面到指定位置
    // 查找目标位置是否存在专辑图片，如果已经存在了，则不写入，否则写入
    QFile coverFile(storageServer->absoluteFilePath(root + "/" + coverDirName + "/" + targetMd5));
    if (!coverFile.exists()) {
        QByteArray albumCover = getAlbumCover(mergedFile, fileName.mid(fileName.lastIndexOf('.') + 1).toLower());
        if (!albumCover.isEmpty()) {
            if (storageServer->writeFile(coverFile, albumCover)) {
                qDebug() << "Album cover written to: " << coverFile.fileName();
            }
        } else {
            qDebug() << "No album cover found";
        }
    } else {
        qDebug() << "Cover file already exists: " << coverFile.fileName();
    }

    // 最终返回写入到元数据表中的 uuid
    return uuid;
}

RogalunaMusicServer::AudioMetadata RogalunaMusicServer::parseAudioFile(QFile &file)
{
    AudioMetadata metadata;

    // 设置文件名
    if (!file.exists()) {
        qWarning() << "File does not exist: " << file.fileName();
        return metadata;
    }

    // 使用 TagLib 解析元数据
    TagLib::FileRef f(file.fileName().toUtf8().data());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();

        // 获取艺术家名称
        metadata.artist = QString::fromStdString(tag->artist().to8Bit(true));

        // 获取专辑名称
        metadata.album = QString::fromStdString(tag->album().to8Bit(true));

        // 获取标题
        metadata.title = QString::fromStdString(tag->title().to8Bit(true));

        // 获取曲目编号
        metadata.trackNumber = tag->track();

        // 获取流派
        metadata.genre = QString::fromStdString(tag->genre().to8Bit(true));

        // 获取发行年份
        metadata.year = tag->year();

        // 获取音频评论
        metadata.comment = QString::fromStdString(tag->comment().to8Bit(true));
    }

    // 获取音频时长和比特率
    if (f.audioProperties()) {
        TagLib::AudioProperties *properties = f.audioProperties();
        int seconds = properties->lengthInSeconds() % 60;
        int minutes = (properties->lengthInSeconds() / 60) % 60;
        int hours = properties->lengthInSeconds() / 3600;

        if (hours > 0) {
            metadata.duration = QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'));
        } else {
            metadata.duration = QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'));
        }

        metadata.bitrate = properties->bitrate();
    }

    return metadata;
}

QByteArray RogalunaMusicServer::getAlbumCover(QFile &file, const QString &type)
{
    std::unique_ptr<AlbumCoverHandler> handler;

    if (type == "mp3") {
        handler = std::make_unique<MP3AlbumCoverHandler>();
    } else if (type == "flac") {
        handler = std::make_unique<FLACAlbumCoverHandler>();
    }

    if (handler) {
        return handler->getAlbumCover(file);
    }
    return QByteArray();
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
