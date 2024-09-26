#ifndef FETCHFILEDIRECTLINKHANDLER_H
#define FETCHFILEDIRECTLINKHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {

class FetchFileDirectLinkHandler
{
public:
    FetchFileDirectLinkHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // FETCHFILEDIRECTLINKHANDLER_H
