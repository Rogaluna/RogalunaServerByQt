#ifndef CONFIGFILEHANDLER_H
#define CONFIGFILEHANDLER_H

#include <QList>

namespace RogalunaConfigurator {

using ConfigData = QMap<QString, QMap<QString, QVariant>>;

class ConfigFileHandler {
public:
    virtual ~ConfigFileHandler() = default;

    virtual void readConfigFile(ConfigData& configGroups) = 0;
    virtual void writeConfigFile(const ConfigData& configGroups) = 0;
    virtual bool isConfigFileValid() const = 0;
};

}

#endif // CONFIGFILEHANDLER_H
