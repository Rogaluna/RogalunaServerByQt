#ifndef GETBOOKREADPROGRESSHANDLER_H
#define GETBOOKREADPROGRESSHANDLER_H

#include <QHttpServerResponse>


class GetBookReadProgressHandler
{
public:
    GetBookReadProgressHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETBOOKREADPROGRESSHANDLER_H
