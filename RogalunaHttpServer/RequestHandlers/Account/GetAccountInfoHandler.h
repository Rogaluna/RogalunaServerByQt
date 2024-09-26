#ifndef GETACCOUNTINFOHANDLER_H
#define GETACCOUNTINFOHANDLER_H

#include <QHttpServerResponse>

namespace Account {


class GetAccountInfoHandler
{
public:
    GetAccountInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETACCOUNTINFOHANDLER_H
