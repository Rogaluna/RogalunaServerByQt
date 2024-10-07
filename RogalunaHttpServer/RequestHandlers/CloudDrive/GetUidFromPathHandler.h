#ifndef GETUIDFROMPATHHANDLER_H
#define GETUIDFROMPATHHANDLER_H

#include <QHttpServerResponse>

class GetUidFromPathHandler
{
public:
    GetUidFromPathHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETUIDFROMPATHHANDLER_H
