#include "RogalunaAccountServer.h"

#include <bcrypt.h>
#include <Database/Account/UsersDAO.h>

RogalunaAccountServer::RogalunaAccountServer(RogalunaStorageServer *storageServer, RogalunaDatabaseServer *databaseServer, const QString &root)
    : root(root)
    , storageServer(storageServer)
    , databaseServer(databaseServer)
{
    QString rootPath = storageServer->absoluteFilePath(root);
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        rootDir.mkpath(".");  // 创建根文件夹
    }
}

QJsonObject RogalunaAccountServer::registerAccount(const QString &username, const QString &password)
{
    // bcrypt 进行加密
    int saltRounds = 10;
    QString hashedPassword = QString::fromStdString(bcrypt::generateHash(password.toStdString(), saltRounds));

    Account::UserDAO userDAO(databaseServer->getDatabase());

    return userDAO.registerUser(username, hashedPassword);
}

QJsonObject RogalunaAccountServer::loginAccount(const QString &usernameOrId, const QString &password)
{
    QJsonObject result;

    Account::UserDAO userDAO(databaseServer->getDatabase());
    QJsonObject user;
    bool isId;
    int id = usernameOrId.toInt(&isId);
    if (isId) {
        user = userDAO.getUserById(id);
    } else {
        user = userDAO.getUserByName(usernameOrId);
    }

    // 如果没有找到账户
    if(user.isEmpty()) {
        result["success"] = false;
        result["message"] = "Username or id invalid!";
        return result;
    }

    // 获取了加密密码，使用 bcrypt 验证密码
    bool isPasswordCorrect = bcrypt::validatePassword(password.toStdString(), user["password"].toString().toStdString());

    // 密码错误
    if (!isPasswordCorrect) {
        result["success"] = false;
        result["message"] = "Password error!";
        return result;
    }

    // 验证正确
    result["success"] = true;
    result["data"] = user;
    return result;
}

QJsonObject RogalunaAccountServer::getUserInfo(const QString &targetId)
{
    Account::UserDAO userDAO(databaseServer->getDatabase());
    QJsonObject result = userDAO.getUserById(targetId.toInt());
    result.remove("password");
    return result;
}
