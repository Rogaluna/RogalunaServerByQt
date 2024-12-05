#ifndef POSTALBUMIMAGEHANDLER_H
#define POSTALBUMIMAGEHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {


class PostAlbumImageHandler
{
public:
    PostAlbumImageHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // POSTALBUMIMAGEHANDLER_H
