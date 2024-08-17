#ifndef GETFILEHANDLER_H
#define GETFILEHANDLER_H

#include <WebRequestHandler.h>

class RogalunaStorageServer;

class GetFileHandler : public WebRequestHandler
{
public:
    GetFileHandler(RogalunaStorageServer *storageServer);

    void handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data) override;

private:
    RogalunaStorageServer *storageServer;
};

#endif // GETFILEHANDLER_H
