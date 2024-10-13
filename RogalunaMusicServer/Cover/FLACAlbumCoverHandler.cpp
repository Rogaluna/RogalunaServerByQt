#include "FLACAlbumCoverHandler.h"

#include <taglib/flac/flacfile.h>

QByteArray FLACAlbumCoverHandler::getAlbumCover(QFile &file)
{
    QByteArray albumCover;
    TagLib::FLAC::File flacFile(file.fileName().toUtf8().data());
    if (flacFile.isValid() && flacFile.pictureList().size() > 0) {
        TagLib::FLAC::Picture *picture = flacFile.pictureList().front();
        albumCover = QByteArray::fromRawData(picture->data().data(), picture->data().size());
    }
    return albumCover;
}
