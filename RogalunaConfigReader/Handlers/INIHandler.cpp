#include "INIHandler.h"
#include <QFile>
#include <QFileInfo>
#include <QSettings>

namespace RogalunaConfigurator {

INIHandler::INIHandler(const QString& filePath) : filePath(filePath) {}

void INIHandler::readConfigFile(ConfigData& configData) {
    if (!isConfigFileValid()) {
        qDebug() << "Config file is invalid or missing.";
        return;
    }

    QSettings settings(filePath, QSettings::IniFormat);

    for (const QString& group : settings.childGroups()) {
        settings.beginGroup(group);
        QMap<QString, QVariant> keyValues;

        for (const QString& key : settings.childKeys()) {
            keyValues[key] = settings.value(key);
        }

        configData[group] = keyValues;
        settings.endGroup();
    }
}

void INIHandler::writeConfigFile(const ConfigData& configData) {
    QSettings settings(filePath, QSettings::IniFormat);

    for (auto it = configData.cbegin(); it != configData.cend(); ++it) {
        settings.beginGroup(it.key());

        for (auto keyIt = it.value().cbegin(); keyIt != it.value().cend(); ++keyIt) {
            settings.setValue(keyIt.key(), keyIt.value());
        }

        settings.endGroup();
    }
}

bool INIHandler::isConfigFileValid() const {
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return false; // 文件不存在或路径不是文件
    }

    if (fileInfo.size() == 0) {
        qDebug() << "Config file is empty.";
        return false;
    }

    QFile file(filePath);

    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "Failed to open config file for validation.";
        return false;
    }

    // 验证文件内容
    bool valid = true;
    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();

        // 忽略空行或注释行
        if (line.isEmpty() || line.startsWith(";") || line.startsWith("#") || line.startsWith("[")) {
            continue;
        }

        // 必须包含 '='，且键值对非空
        QStringList parts = line.split('=');
        if (parts.size() != 2 || parts[0].trimmed().isEmpty() || parts[1].trimmed().isEmpty()) {
            valid = false;
            break;
        }
    }

    file.close();
    return valid;
}

}
