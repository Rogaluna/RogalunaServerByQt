#ifndef FETCHFILEDIRECTLINKHANDLER_H
#define FETCHFILEDIRECTLINKHANDLER_H

#include <QHttpServerResponse>

class FetchFileDirectLinkHandler
{
public:
    FetchFileDirectLinkHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // FETCHFILEDIRECTLINKHANDLER_H
