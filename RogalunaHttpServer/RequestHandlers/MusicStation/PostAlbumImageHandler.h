#ifndef POSTALBUMIMAGEHANDLER_H
#define POSTALBUMIMAGEHANDLER_H

#include <QHttpServerResponse>


class PostAlbumImageHandler
{
public:
    PostAlbumImageHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // POSTALBUMIMAGEHANDLER_H
