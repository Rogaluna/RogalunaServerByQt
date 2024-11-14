#ifndef ROGALUNAACCOUNTSERVER_H
#define ROGALUNAACCOUNTSERVER_H

#include "RogalunaAccountServer_global.h"

#include <RogalunaDatabaseServer.h>
#include <RogalunaStorageServer.h>

class ROGALUNAACCOUNTSERVER_EXPORT RogalunaAccountServer
{
public:
    RogalunaAccountServer(RogalunaStorageServer* storageServer, RogalunaDatabaseServer* databaseServer, const QString &root);

    // 注册账户
    QJsonObject registerAccount(const QString &username, const QString &password);

    // 登录账户
    QJsonObject loginAccount(const QString &usernameOrId, const QString &password);

    // 查询账户信息
    QJsonObject getUserInfo(const QString &targetId);

public:
    QString root;                    ///< 文件存储的根目录。

private:
    RogalunaStorageServer* storageServer;
    RogalunaDatabaseServer* databaseServer;
};

#endif // ROGALUNAACCOUNTSERVER_H
