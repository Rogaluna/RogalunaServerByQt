#ifndef GETBOOKINFOHANDLER_H
#define GETBOOKINFOHANDLER_H

#include <QHttpServerResponse>


class GetBookInfoHandler
{
public:
    GetBookInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETBOOKINFOHANDLER_H
