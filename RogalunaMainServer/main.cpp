#include <QCoreApplication>
#include <configfile.h>
#include <rogalunadatabaseserver.h>
#include <Database/testdao.h>
#include <QSettings>
#include <RogalunaWebServer.h>
#include <rogalunastorageserver.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //========================================================================================//

    // 初始化对象存储

    configFile storageConfig("storage_config.ini");
    if (!storageConfig.isConfigFileValid()) {
        storageConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("Storage", {
                {"root", "E:/Projects/Qt/RogalunaServerByQt/storage"}
            })
        }));
    }
    QList<ConfigGroup> storageConfigGroupsRead;
    storageConfig.readConfigFile(storageConfigGroupsRead);

    RogalunaStorageServer rss(ConfigGroup::getConfigValue(storageConfigGroupsRead, "Storage", "root").toString());


    // QObject::connect(&minio, &RogalunaObjectStorageServer::finished, [](const QByteArray &data){
    //     qDebug() << "Buckets:" << data;
    //     QCoreApplication::quit();
    // });

    // QObject::connect(&minio, &RogalunaObjectStorageServer::errorOccurred, [](const QString &error){
    //     qDebug() << "Error:" << error;
    //     QCoreApplication::quit();
    // });

    // minio.listBuckets();

    //========================================================================================//

    configFile dbConfig("db_config.ini");
    if (!dbConfig.isConfigFileValid()) {
        dbConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("Database", {
               {"hostname", "117.72.65.176"},
               {"dbname", "test"},
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

    QSqlDatabase& db = dbServer.getDatabase();
    TestDAO testDAO(db);

    // 查询数据
    QList<QPair<int, QString>> tests = testDAO.getAllTests();

    // 打印查询结果
    for (const QPair<int, QString>& test : tests) {
        qDebug() << "ID:" << test.first << "Name:" << test.second;
    }

    //========================================================================================//

    // 开启http服务

    configFile httpConfig("http_config.ini");
    if (!httpConfig.isConfigFileValid()) {
        httpConfig.writeConfigFile(ConfigGroup::createConfigGroups({
            ConfigGroup("HttpServer", {
                {"dist", "E:\\Projects\\Web\\rogaluna-web\\dist"},
                {"port", 8000},
            })
        }));
    }
    QList<ConfigGroup> httpConfigGroupsRead;
    httpConfig.readConfigFile(httpConfigGroupsRead);

    RogalunaWebServer server(
        ConfigGroup::getConfigValue(httpConfigGroupsRead, "HttpServer", "dist").toString(),
        &rss);

    server.start(ConfigGroup::getConfigValue(httpConfigGroupsRead, "HttpServer", "port").toInt());

    qDebug() << "Success to start server";

    return a.exec();
}
