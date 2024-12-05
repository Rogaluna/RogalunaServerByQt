#ifndef GETALBUMIMAGEHANDLER_H
#define GETALBUMIMAGEHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {

class GetAlbumImageHandler
{
public:
    GetAlbumImageHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETALBUMIMAGEHANDLER_H
