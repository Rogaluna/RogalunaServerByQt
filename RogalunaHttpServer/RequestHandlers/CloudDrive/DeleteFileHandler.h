#ifndef DELETEFILEHANDLER_H
#define DELETEFILEHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {


class DeleteFileHandler
{
public:
    DeleteFileHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // DELETEFILEHANDLER_H
