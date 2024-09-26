#ifndef GETBOOKREADPROGRESSHANDLER_H
#define GETBOOKREADPROGRESSHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetBookReadProgressHandler
{
public:
    GetBookReadProgressHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETBOOKREADPROGRESSHANDLER_H
