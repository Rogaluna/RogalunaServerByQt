#ifndef POSTFILEHANDLER_H
#define POSTFILEHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {

class PostFileHandler
{
public:
    PostFileHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // POSTFILEHANDLER_H
