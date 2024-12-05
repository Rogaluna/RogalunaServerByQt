#ifndef GETMUSICLISTHANDLER_H
#define GETMUSICLISTHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {

class GetMusicListHandler
{
public:
    GetMusicListHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETMUSICLISTHANDLER_H
