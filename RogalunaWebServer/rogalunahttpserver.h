#ifndef ROGALUNAHTTPSERVER_H
#define ROGALUNAHTTPSERVER_H

#include "RogalunaWebServer_global.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <Router.h>

class RogalunaStorageServer;

class ROGALUNAWEBSERVER_EXPORT RogalunaHttpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit RogalunaHttpServer(const QString &webRootPath, RogalunaStorageServer *storageServer, QObject *parent = nullptr);
    void start(quint16 port);
    void setWebRootPath(const QString &path);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    // 处理 Http
    void handleClient();

private:
    void processRequest(QTcpSocket *socket);

    Router router;
};

#endif // ROGALUNAHTTPSERVER_H
