#include "MP3AlbumCoverHandler.h"

#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>

QByteArray MP3AlbumCoverHandler::getAlbumCover(QFile &file)
{
    QByteArray albumCover;
    TagLib::MPEG::File mpegFile(file.fileName().toUtf8().data());
    if (mpegFile.isValid() && mpegFile.ID3v2Tag()) {
        TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
        auto frames = id3v2tag->frameList("APIC");
        if (!frames.isEmpty()) {
            TagLib::ID3v2::AttachedPictureFrame *picFrame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());
            albumCover = QByteArray::fromRawData(picFrame->picture().data(), picFrame->picture().size());
        }
    }
    return albumCover;
}
