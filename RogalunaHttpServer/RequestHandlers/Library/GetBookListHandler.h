#ifndef GETBOOKLISTHANDLER_H
#define GETBOOKLISTHANDLER_H

#include <QHttpServerResponse>

namespace Library {

class GetBookListHandler
{
public:
    GetBookListHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETBOOKLISTHANDLER_H
