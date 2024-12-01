#ifndef INIHANDLER_H
#define INIHANDLER_H

#include "ConfigFileHandler.h"

namespace RogalunaConfigurator {

class INIHandler : public ConfigFileHandler
{
public:
    explicit INIHandler(const QString& filePath);

    void readConfigFile(ConfigData& configData) override;
    void writeConfigFile(const ConfigData& configData) override;
    bool isConfigFileValid() const override;

private:
    QString filePath;
};

}

#endif // INIHANDLER_H
