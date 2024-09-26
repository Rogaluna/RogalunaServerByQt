#ifndef GETFILEHANDLER_H
#define GETFILEHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {

class GetFileHandler
{
public:
    GetFileHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETFILEHANDLER_H
