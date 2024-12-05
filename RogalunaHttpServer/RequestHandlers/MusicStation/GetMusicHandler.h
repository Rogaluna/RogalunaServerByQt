#ifndef GETMUSICHANDLER_H
#define GETMUSICHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {


class GetMusicHandler
{
public:
    GetMusicHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETMUSICHANDLER_H
