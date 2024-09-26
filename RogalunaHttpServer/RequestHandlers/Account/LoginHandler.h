#ifndef LOGINHANDLER_H
#define LOGINHANDLER_H

#include <QHttpServerResponse>


class LoginHandler
{
public:
    LoginHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // LOGINHANDLER_H
