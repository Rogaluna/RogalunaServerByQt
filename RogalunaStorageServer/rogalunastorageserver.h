#ifndef ROGALUNASTORAGESERVER_H
#define ROGALUNASTORAGESERVER_H

#include "RogalunaStorageServer_global.h"

#include <QObject>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

class QTcpSocket;

struct FileInfoStruct {
    QString name;
    QString type;
    QString size;
    QDateTime lastModified;
};

struct FileReadResult {
    QByteArray data;  // 当前读取的文件块数据
    bool success;     // 操作是否成功
    QString error;    // 错误信息
    bool atEnd;       // 是否已读取到文件末尾
};


class ROGALUNASTORAGESERVER_EXPORT RogalunaStorageServer
{
public:
    RogalunaStorageServer(const QString &rootDir, const QString &tempDir, const QString &tempFilePrefix);

    // 删除拷贝构造函数和赋值操作符，防止实例被拷贝
    RogalunaStorageServer(const RogalunaStorageServer&) = delete;
    RogalunaStorageServer& operator=(const RogalunaStorageServer&) = delete;

public:
    // 获取文件的绝对路径
    QString absoluteFilePath(const QString &relativeFilePath) const;

    bool writeFile(QFile &file, const QByteArray &data, qint64 bufferSize = 4096);
    bool deleteFile(const QString &filePath);
    FileReadResult readFile(QFile &file, qint64 bufferSize = 4096);
    QVector<FileInfoStruct> listFiles(const QString &directoryPath, bool includeDirs = true);

    // 获取临时文件夹路径
    QString getTempPath() const { return temp; };

    bool writeTempFile(const QString &tempFileDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5 = "");
    bool mergeTempFile(const QString &tempFileDirName, int totalChunks, const QString &targetDir, const QString &mergeFileName);

    // 清理临时文件夹

private:
    QString getFileType(const QFileInfo &fileInfo);
    QString formatFileSize(qint64 size) const;

private:
    QString root;
    QString temp;
    QString tempFilePrefix;
};

#endif // ROGALUNASTORAGESERVER_H
