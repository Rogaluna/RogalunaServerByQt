#ifndef GETFILELISTHANDLER_H
#define GETFILELISTHANDLER_H

#include <QHttpServerResponse>

class GetFileListHandler
{
public:
    GetFileListHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETFILELISTHANDLER_H
