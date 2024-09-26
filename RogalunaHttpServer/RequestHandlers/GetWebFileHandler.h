#ifndef GETWEBFILEHANDLER_H
#define GETWEBFILEHANDLER_H

#include <QHttpServerResponse>

class GetWebFileHandler
{
public:
    GetWebFileHandler() = default;

    static QHttpServerResponse handleRequest(QUrl arg, const QHttpServerRequest &request);
};

class RedirectToWebHandler
{
public:
    RedirectToWebHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest & /*request*/);
};

#endif // GETWEBFILEHANDLER_H
