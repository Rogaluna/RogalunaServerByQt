#ifndef GETPARENTFOLDERHANDLER_H
#define GETPARENTFOLDERHANDLER_H

#include <QHttpServerResponse>


class GetParentFolderHandler
{
public:
    GetParentFolderHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETPARENTFOLDERHANDLER_H
