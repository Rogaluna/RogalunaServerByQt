#ifndef POSTFILEHANDLER_H
#define POSTFILEHANDLER_H

#include <WebRequestHandler.h>

class RogalunaStorageServer;

class PostFileHandler : public WebRequestHandler
{
public:
    PostFileHandler(RogalunaStorageServer *storageServer);

    void handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data) override;

private:
    RogalunaStorageServer *storageServer;
};

#endif // POSTFILEHANDLER_H
