#ifndef GETFILELISTHANDLER_H
#define GETFILELISTHANDLER_H

#include <WebRequestHandler.h>

class RogalunaStorageServer;

class GetFileListHandler : public WebRequestHandler
{
public:
    GetFileListHandler(RogalunaStorageServer *storageServer);

    void handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data) override;

private:
    RogalunaStorageServer *storageServer;
};

#endif // GETFILELISTHANDLER_H
