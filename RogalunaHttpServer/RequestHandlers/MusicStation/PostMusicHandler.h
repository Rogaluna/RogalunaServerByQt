#ifndef POSTMUSICHANDLER_H
#define POSTMUSICHANDLER_H

#include <QHttpServerResponse>


class PostMusicHandler
{
public:
    PostMusicHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // POSTMUSICHANDLER_H
