#ifndef GETWEBFILEHANDLER_H
#define GETWEBFILEHANDLER_H

#include <QHttpServerResponse>

class GetWebFileHandler
{
public:
    GetWebFileHandler() = default;

    static QHttpServerResponse handleRequest(QUrl arg, const QHttpServerRequest &request);
};

#endif // GETWEBFILEHANDLER_H
