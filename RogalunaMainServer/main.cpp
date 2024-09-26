#include <QCoreApplication>
#include <ConfigFile.h>
#include <QSettings>


#include <RogalunaStorageServer.h>
#include <RogalunaDatabaseServer.h>


#include <Database/TestDAO.h>


#include <RogalunaCloudDriveServer.h>
#include <RogalunaLibraryServer.h>


#include <RogalunaHttpServer.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //========================================================================================//

    // 初始化存储器

    configFile storageConfig("storage_config.ini");
    if (!storageConfig.isConfigFileValid()) {
        storageConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("Storage", {
                {"root", "E:/Projects/Qt/RogalunaServerByQt/storage"},
                {"temp", "E:/Projects/Qt/RogalunaServerByQt/temp"},
                {"tempFilePrefix", "chunk_"}
            })
        }));
    }
    QList<ConfigGroup> storageConfigGroupsRead;
    storageConfig.readConfigFile(storageConfigGroupsRead);
    RogalunaStorageServer rss = RogalunaStorageServer(ConfigGroup::getConfigValue(storageConfigGroupsRead, "Storage", "root").toString(),
                                                      ConfigGroup::getConfigValue(storageConfigGroupsRead, "Storage", "temp").toString(),
                                                      ConfigGroup::getConfigValue(storageConfigGroupsRead, "Storage", "tempFilePrefix").toString());

    //========================================================================================//

    // 初始化数据库

    configFile dbConfig("db_config.ini");
    if (!dbConfig.isConfigFileValid()) {
        dbConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("Database", {
               {"hostname", "117.72.65.176"},
               {"dbname", "rogaluna_database"},
               {"username", "postgres"},
               {"password", "Lxzx546495"},
               {"port", 5432}
            })
        }));
    }
    QList<ConfigGroup> dbConfigGroupsRead;
    dbConfig.readConfigFile(dbConfigGroupsRead);

    RogalunaDatabaseServer dbServer(
        ConfigGroup::getConfigValue(dbConfigGroupsRead, "Database", "hostname").toString(),
        ConfigGroup::getConfigValue(dbConfigGroupsRead, "Database", "dbname").toString(),
        ConfigGroup::getConfigValue(dbConfigGroupsRead, "Database", "username").toString(),
        ConfigGroup::getConfigValue(dbConfigGroupsRead, "Database", "password").toString(),
        ConfigGroup::getConfigValue(dbConfigGroupsRead, "Database", "port").toInt());
    // RogalunaDatabaseServer dbServer("117.72.65.176", "test", "postgres", "Lxzx546495", 5432);
    if (!dbServer.connect())
    {
        qDebug() << "Failed to connect to database";
        return -1;
    }

    //========================================================================================//

    // 初始化云盘服务

    configFile cloudDriveConfig("cloud_drive_config.ini");
    if (!cloudDriveConfig.isConfigFileValid()) {
        cloudDriveConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("CloudDrive", {
                {"root", "cloudDrive"},
            })
        }));
    }
    QList<ConfigGroup> cloudDriveConfigGroupsRead;
    cloudDriveConfig.readConfigFile(cloudDriveConfigGroupsRead);

    RogalunaCloudDriveServer cloudDriveServer(&rss, ConfigGroup::getConfigValue(cloudDriveConfigGroupsRead, "CloudDrive", "root").toString());

    // 初始化图书馆服务

    configFile libraryConfig("library_config.ini");
    if (!libraryConfig.isConfigFileValid()) {
        libraryConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("Library", {
               {"root", "library"},
               {"categoryRootId", "1"},
           })
        }));
    }
    QList<ConfigGroup> libraryConfigGroupsRead;
    libraryConfig.readConfigFile(libraryConfigGroupsRead);
    RogalunaLibraryServer libraryServer(
        &rss,
        &dbServer,
        ConfigGroup::getConfigValue(libraryConfigGroupsRead, "Library", "root").toString(),
        ConfigGroup::getConfigValue(libraryConfigGroupsRead, "Library", "categoryRootId").toInt());

    //========================================================================================//

    // 开启http服务

    configFile httpConfig("http_config.ini");
    if (!httpConfig.isConfigFileValid()) {
        httpConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("HttpServer", {
                {"dist", "E:\\Projects\\Web\\rogaluna-web\\dist"},
                {"port", 8000},
                {"algorithm", "HS256"},
                {"secretKey", "rogaluna"}
            })
        }));
    }
    QList<ConfigGroup> httpConfigGroupsRead;
    httpConfig.readConfigFile(httpConfigGroupsRead);

    RogalunaHttpServer server(
        ConfigGroup::getConfigValue(httpConfigGroupsRead, "HttpServer", "dist").toString(),
        ConfigGroup::getConfigValue(httpConfigGroupsRead, "HttpServer", "algorithm").toString(),
        ConfigGroup::getConfigValue(httpConfigGroupsRead, "HttpServer", "secretKey").toString(),
        &rss,
        &dbServer,

        &cloudDriveServer,
        &libraryServer);

    server.start(ConfigGroup::getConfigValue(httpConfigGroupsRead, "HttpServer", "port").toInt());

    return a.exec();
}
