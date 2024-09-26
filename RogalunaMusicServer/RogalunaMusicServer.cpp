#include "RogalunaMusicServer.h"

#include <Database/MusicStation/AlbumsDAO.h>
#include <Database/MusicStation/ContentDAO.h>
#include <Database/MusicStation/MetadataDAO.h>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include <AlbumCoverHandler.h>

#include <QJsonArray>

#include <Cover/FLACAlbumCoverHandler.h>
#include <Cover/MP3AlbumCoverHandler.h>

RogalunaMusicServer::RogalunaMusicServer(
    RogalunaStorageServer* storageServer,
    RogalunaDatabaseServer* databaseServer,
    const QString &root,
    const QString &musicDirName,
    const QString &coverDirName)
    : root(root)
    , musicDirName(musicDirName)
    , coverDirName(coverDirName)
    , storageServer(storageServer)
    , databaseServer(databaseServer)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(QDir::separator());  // 创建根文件夹
    }

    QString musicDirPath = rootPath + QDir::separator() + musicDirName;
    QDir musicDir(musicDirPath);
    if (!musicDir.exists()) {
        musicDir.mkpath(QDir::separator()); // 构造音乐目录的路径
    }

    QString coverDirPath = rootPath + QDir::separator() + coverDirName;
    QDir coverDir(coverDirPath);
    if (!coverDir.exists()) {
        coverDir.mkpath(QDir::separator()); // 构造封面目录的路径
    }
}

QString RogalunaMusicServer::getMusicFilePath(const QString &musicMd5)
{
    return storageServer->absoluteFilePath(root + QDir::separator() + musicDirName + QDir::separator() + musicMd5);
}

QJsonArray RogalunaMusicServer::getMusicList(const EMusicQueryType &oper, const QString &query)
{
    MusicStation::MetadataDAO metadataDao(databaseServer->getDatabase());
    QJsonArray musicList = QJsonArray();


    switch (oper) {
    case EMusicQueryType::E_RANDOM:
    {
        // 随机获取音乐，不使用参数 query
        musicList = metadataDao.getRandomMetadata();
    }
    break;
    case EMusicQueryType::E_PRIVATE:
    {
        // 获取私有音乐，参数 query 指示用户的 id
        musicList = metadataDao.getMetadataByUserIdAndPublished(query.toInt(), false);
    }
    break;
    case EMusicQueryType::E_BYALBUM:
    {
        // 获取专辑内的音乐，参数 query 指示专辑 id
        musicList = metadataDao.getMetadataByAlbumId(query);
    }
    break;
    case EMusicQueryType::E_BYUSERID:
    {
        // 获取用户收藏的全部音乐，参数 query 指示用户 id
        musicList = metadataDao.getMetadataByUserId(query.toInt());
    }
    break;
    default:
        break;
    }

    return musicList;
}

bool RogalunaMusicServer::uploadChunk(const QString &tempDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5)
{
    // 将块数据写入临时文件夹
    const QString &targetPath = storageServer->absoluteFilePath(tempDirName + QDir::separator() + QString::number(chunkIndex), storageServer->temp);
    return storageServer->writeFile(targetPath, chunkData, chunkMd5);
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
    QString musicDirPath = root + QDir::separator() + musicDirName;
    QString coverDirPath = root + QDir::separator() + coverDirName;

    // 合并临时文件夹中的文件块
    if (!storageServer->mergeTempFile(tempDirName, totalChunks, musicDirPath, targetMd5)) {
        qWarning() << "Failed to merge chunks";
        return QString();
    }

    QFile mergedFile(storageServer->absoluteFilePath(musicDirPath + QDir::separator() + targetMd5));
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
    QString fileType = fileName.mid(fileName.lastIndexOf('.') + 1).toLower();
    QString uuid = metadataDao.insertMetadata(
        userId,
        fileType,
        audioMetadata.title,
        audioMetadata.duration,
        audioMetadata.artist,
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
    QFile coverFile(storageServer->absoluteFilePath(coverDirPath + QDir::separator() + uuid));
    if (!coverFile.exists()) {
        QByteArray albumCover = extractAlbumCover(mergedFile, fileType);
        if (!albumCover.isEmpty()) {
            if (coverFile.open(QIODevice::WriteOnly)) {  // 打开文件以写入模式
                if (storageServer->writeFile(coverFile, albumCover)) {
                    qDebug() << "Album cover written to: " << coverFile.fileName();
                } else {
                    qDebug() << "Failed to write album cover to: " << coverFile.fileName();
                }
                coverFile.close();  // 写入后关闭文件
            } else {
                qWarning() << "Failed to open file for writing: " << coverFile.fileName();
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

bool RogalunaMusicServer::saveAlbumCover(const QString &musicUid, const QByteArray &cover)
{
    QString coverDirPath = root + QDir::separator() + coverDirName;

    return storageServer->writeFile(
        storageServer->absoluteFilePath(coverDirPath + QDir::separator() + musicUid),
        cover
        );
}

QByteArray RogalunaMusicServer::getCoverImage(const QString id)
{
    QString coverDirPath = root + QDir::separator() + coverDirName;

    FileReadResult result = storageServer->readFile(
        storageServer->absoluteFilePath(coverDirPath + QDir::separator() + id),
        0, 1024 * 1024 * 10);
    const QByteArray &fileData = result.data;

    return fileData;
}

QJsonArray RogalunaMusicServer::getMusicMetadata(const QStringList &uids)
{
    MusicStation::MetadataDAO metadataDao(databaseServer->getDatabase());
    return metadataDao.getMetadataByUid(uids);
}

QJsonArray RogalunaMusicServer::getAlbumsMetadata(const QStringList &ids)
{
    MusicStation::AlbumsDAO albumDao(databaseServer->getDatabase());
    return albumDao.getAlbumsById(ids);
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
        // int seconds = properties->lengthInSeconds() % 60;
        // int minutes = (properties->lengthInSeconds() / 60) % 60;
        // int hours = properties->lengthInSeconds() / 3600;

        // if (hours > 0) {
        //     metadata.duration = QString("%1:%2:%3")
        //     .arg(hours, 2, 10, QChar('0'))
        //         .arg(minutes, 2, 10, QChar('0'))
        //         .arg(seconds, 2, 10, QChar('0'));
        // } else {
        //     metadata.duration = QString("%1:%2")
        //     .arg(minutes, 2, 10, QChar('0'))
        //         .arg(seconds, 2, 10, QChar('0'));
        // }

        metadata.duration = properties->lengthInSeconds();
        metadata.bitrate = properties->bitrate();
    }

    return metadata;
}

QByteArray RogalunaMusicServer::extractAlbumCover(QFile &file, const QString &type)
{
    std::unique_ptr<AlbumCoverHandler> handler;

    if (type == "mp3") {
        handler = std::make_unique<MP3AlbumCoverHandler>();
    } else if (type == "flac") {
        handler = std::make_unique<FLACAlbumCoverHandler>();
    }

    if (handler) {
        return handler->extractAlbumCover(file);
    }
    return QByteArray();
}
