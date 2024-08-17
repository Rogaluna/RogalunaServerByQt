#include "configfile.h"

#include <QFileInfo>
#include <QSettings>

configFile::configFile(const QString& configFileName)
    : configFileName(configFileName)
{}

bool configFile::isConfigFileValid()
{
    QFileInfo fileInfo(configFileName);
    if (fileInfo.isFile())
    {
        // 存在情况下，检查内容是否完整
        QFile file(configFileName);
        if (file.size() == 0)
        {
            return false;
        }
        else
        {
            if (file.open(QFile::ReadOnly)) {
                bool ok = true;
                while (!file.atEnd()) {
                    QString line = file.readLine().trimmed();
                    QStringList list = line.split('=');

                    if (list.count() == 2) {
                        if (list.at(1).isEmpty()) {
                            ok = false;
                            break;
                        }
                    }
                }

                file.close(); // 关闭文件
                if (!ok) {
                    return false;
                }
            } else {
                return false;
            }
        }
        // 如果配置文件不完整,则以初始值继续运行,并生成配置文件

        return true;
    }
    else
    {
        return false;
    }
}

void configFile::readConfigFile(QList<ConfigGroup>& configGroups)
{
    //先判断配置文件是否存在
    if (!isConfigFileValid()) {
        qDebug() << "Config file is invalid or missing.";
        return;
    }

    QSettings set(configFileName, QSettings::IniFormat);

    QStringList groups = set.childGroups();

    for (const QString& group : groups) {
        set.beginGroup(group);
        QStringList keys = set.childKeys();
        ConfigGroup configGroup;
        configGroup.groupName = group;
        for (const QString& key : keys) {
            QVariant value = set.value(key);
            configGroup.keyValues.insert(key, value);
            qDebug() << "Group:" << group << ", Key:" << key << ", Value:" << value.toString();
        }
        configGroups.append(configGroup);
        set.endGroup();
    }
}

void configFile::writeConfigFile(const QList<ConfigGroup>& configGroups)
{
    QSettings set(configFileName, QSettings::IniFormat);

    for (const ConfigGroup& group : configGroups) {
        set.beginGroup(group.groupName);
        for (auto it = group.keyValues.begin(); it != group.keyValues.end(); ++it) {
            set.setValue(it.key(), it.value());
        }
        set.endGroup();
    }

}

QVariant ConfigGroup::getConfigValue(const QList<ConfigGroup> &configGroups, const QString &groupName, const QString &keyName)
{
    for (const ConfigGroup& group : configGroups) {
        if (group.groupName == groupName) {
            if (group.keyValues.contains(keyName)) {
                return group.keyValues.value(keyName);
            } else {
                qDebug() << "Key:" << keyName << "not found in group:" << groupName;
                return QVariant();
            }
        }
    }
    qDebug() << "Group:" << groupName << "not found";
    return QVariant();
}
