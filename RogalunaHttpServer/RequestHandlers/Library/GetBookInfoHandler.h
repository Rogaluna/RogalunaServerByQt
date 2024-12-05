#ifndef GETBOOKINFOHANDLER_H
#define GETBOOKINFOHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetBookInfoHandler
{
public:
    GetBookInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETBOOKINFOHANDLER_H
