#include "RogalunaConfigReader.h"

#include <QFileInfo>

#include <Handlers/INIHandler.h>

RogalunaConfigReader::RogalunaConfigReader() {}

std::unique_ptr<RogalunaConfigurator::ConfigFileHandler> RogalunaConfigReader::createHandler(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();

    if (extension == "ini") {
        return std::make_unique<RogalunaConfigurator::INIHandler>(filePath);
    }

    // 添加其他格式
    // else if (extension == "json") {
    //     return std::make_unique<JSONHandler>(filePath);
    // }

    // 格式不支持，返回 nullptr
    return nullptr;
}


