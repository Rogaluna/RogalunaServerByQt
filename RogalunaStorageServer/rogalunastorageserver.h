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


class ROGALUNASTORAGESERVER_EXPORT RogalunaStorageServer : public QObject
{
    Q_OBJECT
public:
    RogalunaStorageServer(const QString &rootDir, QObject *parent = nullptr);

    QString absoluteFilePath(const QString &relativeFilePath) const;

    bool writeFile(QFile &file, const QByteArray &data, qint64 bufferSize = 4096);
    bool deleteFile(const QString &filePath);
    FileReadResult readFile(QFile &file, qint64 bufferSize = 4096);
    QVector<FileInfoStruct> listFiles(const QString &directoryPath, bool includeDirs = true);

private:
    QString getFileType(const QFileInfo &fileInfo);
    QString formatFileSize(qint64 size) const;

private:
    QString root;
};

#endif // ROGALUNASTORAGESERVER_H
