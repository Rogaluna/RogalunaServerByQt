#include "MP3AlbumCoverHandler.h"

#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>
#include <QFileStream.h>

QByteArray MP3AlbumCoverHandler::extractAlbumCover(QFile &file)
{
    TagLib::IOStream *stream = new QFileStream(file);
    TagLib::MPEG::File mpegFile(stream, TagLib::ID3v2::FrameFactory::instance(), false);
    QByteArray imageData;

    TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
    if (id3v2tag) {
        TagLib::ID3v2::FrameList frames = id3v2tag->frameList("APIC");
        if (!frames.isEmpty()) {
            for (auto it = frames.begin(); it != frames.end(); ++it) {
                auto *picFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(*it);
                if (picFrame) {
                    TagLib::ByteVector data = picFrame->picture();
                    imageData = QByteArray(data.data(), data.size());
                    break;
                }
            }
        }
    }

    delete stream;
    return imageData;
}
