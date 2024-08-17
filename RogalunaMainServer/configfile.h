#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QObject>
#include <QMap>

struct ConfigGroup {
    QString groupName;
    QMap<QString, QVariant> keyValues;

    ConfigGroup() = default;
    ConfigGroup(const QString& name, std::initializer_list<std::pair<QString, QVariant>> list)
        : groupName(name), keyValues(list) {}

    static QList<ConfigGroup> createConfigGroups(std::initializer_list<ConfigGroup> groups) {
        return QList<ConfigGroup>(groups);
    }

    static QVariant getConfigValue(const QList<ConfigGroup>& configGroups, const QString& groupName, const QString& keyName);
};

class configFile
{
public:
    configFile(const QString& configFileName);
    //需要一个文件名,不用设置保存路径，会自动生成在exe同级目录下
    QString configFileName;

    //方法
public:
    bool isConfigFileValid();

    void readConfigFile(QList<ConfigGroup>& configGroups);
    void writeConfigFile(const QList<ConfigGroup>& configGroups);
};

#endif // CONFIGFILE_H
