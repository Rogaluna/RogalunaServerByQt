#include <QCoreApplication>

#include <RogalunaStorageServer.h>
#include <RogalunaDatabaseServer.h>


#include <RogalunaConfigReader.h>


#include <RogalunaAccountServer.h>
#include <RogalunaCloudDriveServer.h>
#include <RogalunaLibraryServer.h>
#include <RogalunaMusicServer.h>


#include <RogalunaHttpServer.h>


#define REQUIRE_INPUT(configData, group, key) \
while (true) { \
        qDebug() << "Enter" << #group << #key << "value (require):"; \
        QTextStream in(stdin); \
        QString input = in.readLine().trimmed(); \
        if (!input.isEmpty()) { \
            configData[#group][#key] = input; \
            break; \
    } else { \
            qDebug() << #key << "cannot be empty! Please input a valid value."; \
    } \
}

#define OPTIONAL_INPUT(configData, group, key) \
{ \
        qDebug() << "Enter" << #group << #key << "value (default:" << configData[#group][#key] << "):"; \
        QTextStream in(stdin); \
        QString input = in.readLine().trimmed(); \
        if (!input.isEmpty()) { \
            configData[#group][#key] = input; \
    } else { \
            qDebug() << "Use default value:" << configData[#group][#key]; \
    } \
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Initializing...";

    auto configHandler = CREATE_CONFIG_HANDLER("config.ini");

    RogalunaConfigurator::ConfigData defaultConfig;
    // 存储服务
    defaultConfig["Storage"] = {
        {"root", "storage"},
        {"temp", "temp"},
        {"bufferSize", 4096}
    };

    // 数据库服务
    defaultConfig["Database"] = {
        {"hostname", ""},
        {"dbname", ""},
        {"username", "postgres"},
        {"password", ""},
        {"port", 5432},
        {"interval", 5000},
        {"maxReconnectAttempts", 5},
        {"reconnectInterval", 3000}
    };

    // 账户服务
    defaultConfig["Account"] = {
        {"root", "account"},
        {"saltRounds", 10}
    };

    // 云盘服务
    defaultConfig["CloudDrive"] = {
        {"root", "cloudDrive"}
    };

    // 图书馆服务
    defaultConfig["Library"] = {
        {"root", "library"},
        {"bookDirName", "books"},
        {"coverDirName", "covers"},
        {"maxRangeSize", 10},
        {"maxSingleResSize", 10485760},
        {"categoryRootId", 1}
    };

    // 音乐台服务
    defaultConfig["MusicStation"] = {
        {"root", "musicStation"},
        {"musicDirName", "music"},
        {"coverDirName", "covers"}
    };

    // HTTP 服务
    defaultConfig["HttpServer"] = {
        {"dist", "www/dist"},
        {"algorithm", "HS256"},
        {"secretKey", ""}, // 秘密密钥，生成 token 所必须的加密密钥
        {"httpPort", 80},
        {"enableSsl", true},
        {"httpsPort", 443},
        {"certFilePath", "fullchain.crt"},
        {"keyFilePath", "private.pem"},
        {"sslEncryptAlg", "Ec"}
    };

    VALIDATE_CONFIG(configHandler, defaultConfig, [](RogalunaConfigurator::ConfigData& configData) {
        QTextStream in(stdin);

        // // 存储服务
        // if (configData.contains("Storage")) {
        //     OPTIONAL_INPUT(configData, Storage, root);
        //     OPTIONAL_INPUT(configData, Storage, temp);
        // }

        // 数据库服务
        if (configData.contains("Database")) {
            REQUIRE_INPUT(configData, Database, hostname);
            REQUIRE_INPUT(configData, Database, dbname);
            OPTIONAL_INPUT(configData, Database, username);
            REQUIRE_INPUT(configData, Database, password);
            OPTIONAL_INPUT(configData, Database, port);
        }

        // HTTP 服务
        if (configData.contains("HttpServer")) {
            // REQUIRE_INPUT(configData, HttpServer, certFilePath);
            // REQUIRE_INPUT(configData, HttpServer, keyFilePath);
            REQUIRE_INPUT(configData, HttpServer, secretKey);
        }
    });

    RogalunaConfigurator::ConfigData configData = READ_CONFIG(configHandler);

    qDebug() << "Initialization completed, start server...";

    //========================================================================================//

    // 初始化存储器
    RogalunaStorageServer rss = RogalunaStorageServer(
        configData["Storage"].value("root").toString(),
        configData["Storage"].value("temp").toString(),
        configData["Storage"].value("bufferSize").toInt());

    //========================================================================================//

    // 初始化数据库

    RogalunaDatabaseServer dbServer(
        configData["Database"].value("hostname").toString(),
        configData["Database"].value("dbname").toString(),
        configData["Database"].value("username").toString(),
        configData["Database"].value("password").toString(),
        configData["Database"].value("port").toInt(),
        configData["Database"].value("interval").toInt(),
        configData["Database"].value("maxReconnectAttempts").toInt(),
        configData["Database"].value("reconnectInterval").toInt());
    // RogalunaDatabaseServer dbServer("117.72.65.176", "test", "postgres", "Lxzx546495", 5432);
    if (!dbServer.connect())
    {
        qDebug() << "Failed to connect to database";
        return -1;
    }

    //========================================================================================//

    // 初始化账户服务
    RogalunaAccountServer accountServer(
        &rss,
        &dbServer,
        configData["Account"].value("root").toString(),
        configData["Account"].value("saltRounds").toInt());

    // 初始化云盘服务
    RogalunaCloudDriveServer cloudDriveServer(
        &rss,
        &dbServer,
        configData["CloudDrive"].value("root").toString());

    // 初始化图书馆服务
    RogalunaLibraryServer libraryServer(
        &rss,
        &dbServer,
        configData["Library"].value("root").toString(),
        configData["Library"].value("bookDirName").toString(),
        configData["Library"].value("coverDirName").toString(),
        configData["Library"].value("maxRangeSize").toInt(),
        configData["Library"].value("maxSingleResSize").toInt(),
        configData["Library"].value("categoryRootId").toInt());

    // 初始化音乐台服务
    RogalunaMusicServer musicServer(
        &rss,
        &dbServer,
        configData["MusicStation"].value("root").toString(),
        configData["MusicStation"].value("musicDirName").toString(),
        configData["MusicStation"].value("coverDirName").toString());

    //========================================================================================//

    // 开启http服务
    RogalunaHttpServer server(
        configData["HttpServer"].value("dist").toString(),
        configData["HttpServer"].value("algorithm").toString(),
        configData["HttpServer"].value("secretKey").toString(),
        configData["HttpServer"].value("httpPort").toInt(),
        configData["HttpServer"].value("enableSsl").toBool(),
        configData["HttpServer"].value("httpsPort").toInt(),
        configData["HttpServer"].value("certFilePath").toString(),
        configData["HttpServer"].value("keyFilePath").toString(),
        configData["HttpServer"].value("sslEncryptAlg").toString());

    server.setStorageServer(&rss);
    server.setDatabaseServer(&dbServer);
    server.setAccountServer(&accountServer);
    server.setCloudDriveServer(&cloudDriveServer);
    server.setLibraryServer(&libraryServer);
    server.setMusicServer(&musicServer);

    server.postInitialization();

    server.start();

    return a.exec();
}
