#ifndef POSTMUSICHANDLER_H
#define POSTMUSICHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {


class PostMusicHandler
{
public:
    PostMusicHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // POSTMUSICHANDLER_H
