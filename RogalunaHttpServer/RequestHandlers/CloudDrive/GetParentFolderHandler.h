#ifndef GETPARENTFOLDERHANDLER_H
#define GETPARENTFOLDERHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {


class GetParentFolderHandler
{
public:
    GetParentFolderHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETPARENTFOLDERHANDLER_H
