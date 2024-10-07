#ifndef POSTFILEHANDLER_H
#define POSTFILEHANDLER_H

#include <QHttpServerResponse>

class PostFileHandler
{
public:
    PostFileHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // POSTFILEHANDLER_H
