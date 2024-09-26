#ifndef GETALBUMSINFOHANDLER_H
#define GETALBUMSINFOHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {

class GetAlbumsInfoHandler
{
public:
    GetAlbumsInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETALBUMSINFOHANDLER_H
