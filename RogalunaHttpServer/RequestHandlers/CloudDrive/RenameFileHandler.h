#ifndef RENAMEFILEHANDLER_H
#define RENAMEFILEHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {


class RenameFileHandler
{
public:
    RenameFileHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // RENAMEFILEHANDLER_H
