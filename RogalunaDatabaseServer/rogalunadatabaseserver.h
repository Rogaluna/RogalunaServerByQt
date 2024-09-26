#ifndef ROGALUNADATABASESERVER_H
#define ROGALUNADATABASESERVER_H

#include "RogalunaDatabaseServer_Global.h"
#include <QtSql/QSqlDatabase>

class ROGALUNADATABASESERVER_EXPORT RogalunaDatabaseServer
{
public:
    RogalunaDatabaseServer(const QString& hostname, const QString& dbname, const QString& username, const QString& password, int port = 5432);
    ~RogalunaDatabaseServer();

    bool connect();
    void disconnect();

    QSqlDatabase& getDatabase();

private:
    QSqlDatabase db;
    QString hostname;
    QString dbname;
    QString username;
    QString password;
    int port;
};

#endif // ROGALUNADATABASESERVER_H
