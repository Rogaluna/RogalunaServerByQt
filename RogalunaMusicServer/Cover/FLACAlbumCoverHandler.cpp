#include "FLACAlbumCoverHandler.h"

#include <taglib/flac/flacfile.h>

#include <QFileStream.h>

QByteArray FLACAlbumCoverHandler::extractAlbumCover(QFile &file)
{
    TagLib::IOStream *stream = new QFileStream(file);
    TagLib::FLAC::File flacFile(stream, false);
    QByteArray imageData;

    const auto &pictureList = flacFile.pictureList();
    if (!pictureList.isEmpty()) {
        TagLib::FLAC::Picture *pic = pictureList.front();
        TagLib::ByteVector data = pic->data();
        imageData = QByteArray(data.data(), data.size());
    }

    delete stream;
    return imageData;
}
