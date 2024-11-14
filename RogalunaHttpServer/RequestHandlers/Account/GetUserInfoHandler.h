#ifndef GETUSERINFOHANDLER_H
#define GETUSERINFOHANDLER_H

#include <QHttpServerResponse>


class GetUserInfoHandler
{
public:
    GetUserInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETUSERINFOHANDLER_H
