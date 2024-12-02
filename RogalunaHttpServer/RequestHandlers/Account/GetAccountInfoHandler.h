#ifndef GETACCOUNTINFOHANDLER_H
#define GETACCOUNTINFOHANDLER_H

#include <QHttpServerResponse>


class GetAccountInfoHandler
{
public:
    GetAccountInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETACCOUNTINFOHANDLER_H
