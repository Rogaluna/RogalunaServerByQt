#ifndef GETWEBFILEHANDLER_H
#define GETWEBFILEHANDLER_H

#include <WebRequestHandler.h>

class GetWebFileHandler : public WebRequestHandler
{
public:
    GetWebFileHandler(const QString &rootPath);

    void handleRequest(QTcpSocket *socket, const QString &path, const QUrlQuery &query, const QByteArray &data) override;

private:
    QString webRootPath;

    QString getContentType(const QString &filePath);
};

#endif // GETWEBFILEHANDLER_H
