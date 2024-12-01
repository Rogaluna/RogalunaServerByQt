#ifndef ROGALUNACONFIGREADER_H
#define ROGALUNACONFIGREADER_H

#include "RogalunaConfigReader_Global.h"

#include <ConfigFileHandler.h>

class ROGALUNACONFIGREADER_EXPORT RogalunaConfigReader
{
public:
    RogalunaConfigReader();

    // 创建配置文件句柄
    static std::unique_ptr<RogalunaConfigurator::ConfigFileHandler> createHandler(const QString& filePath);
};

#define CREATE_CONFIG_HANDLER(filePath) RogalunaConfigReader::createHandler(filePath)

#define VALIDATE_CONFIG(handler, defaultConfig, inputLogic) do { \
    if (!handler) { \
            qDebug() << "Handler pointer is null!"; \
            return -1; \
    } \
        \
        if (!handler->isConfigFileValid()) { \
            qDebug() << "Config file is invalid or not exist!"; \
            RogalunaConfigurator::ConfigData configGroups = defaultConfig; \
            inputLogic(configGroups); \
            handler->writeConfigFile(configGroups); \
    } \
    } while (0)

#define READ_CONFIG(handler) ([&]() -> RogalunaConfigurator::ConfigData { \
    RogalunaConfigurator::ConfigData configGroups; \
    if (handler) { \
        handler->readConfigFile(configGroups); \
    } else { \
        qDebug() << "Handler is null, cannot read configuration."; \
    } \
    return configGroups; \
})()

#endif // ROGALUNACONFIGREADER_H
