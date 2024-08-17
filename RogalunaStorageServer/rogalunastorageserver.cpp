#include "rogalunastorageserver.h"

#include <QFile>

RogalunaStorageServer::RogalunaStorageServer(const QString &rootDir, QObject *parent)
    : QObject(parent), root(rootDir)
{
    if (!root.endsWith('/'))
    {
        root += '/';
    }
}

QString RogalunaStorageServer::absoluteFilePath(const QString &relativeFilePath) const
{
    return root + relativeFilePath;
}

bool RogalunaStorageServer::writeFile(QFile &file, const QByteArray &data, qint64 bufferSize)
{
    if (!file.isOpen() || !(file.openMode() & QIODevice::WriteOnly)) {
        return false;  // 文件未打开或未以写入模式打开
    }

    qint64 bytesWritten = 0;
    qint64 totalBytes = data.size();

    while (bytesWritten < totalBytes) {
        qint64 chunkSize = qMin(bufferSize, totalBytes - bytesWritten);
        qint64 written = file.write(data.mid(bytesWritten, chunkSize));
        if (written == -1) {
            return false;  // 写入失败
        }
        bytesWritten += written;
    }

    return bytesWritten == totalBytes;
}

bool RogalunaStorageServer::deleteFile(const QString &relativeFilePath)
{
    QString filePath = absoluteFilePath(relativeFilePath);
    QFile file(filePath);
    if (file.exists())
    {
        return file.remove();
    }
    return false;
}

FileReadResult RogalunaStorageServer::readFile(QFile &file, qint64 bufferSize) {
    FileReadResult result;

    if (!file.isOpen()) {
        result.success = false;
        result.error = "File is not open";
        return result;
    }

    // 读取文件块
    result.data = file.read(bufferSize);
    result.success = !result.data.isEmpty();
    result.atEnd = file.atEnd();

    if (!result.success && !result.atEnd) {
        result.error = "Failed to read file";
    }

    return result;
}

QVector<FileInfoStruct> RogalunaStorageServer::listFiles(const QString &relativeDirPath, bool includeDirs)
{
    QDir dir(absoluteFilePath(relativeDirPath));
    QVector<FileInfoStruct> entryList;


    QFileInfoList list = dir.entryInfoList(includeDirs ? (QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot) : QDir::Files);
    for (const QFileInfo &info : list) {
        FileInfoStruct entryInfo;
        entryInfo.name = info.fileName();
        entryInfo.type = getFileType(info);
        entryInfo.size = formatFileSize(info.isDir() ? 0 : info.size());
        entryInfo.lastModified = info.lastModified();

        entryList.push_back(entryInfo);
    }

    return entryList;
}

QString RogalunaStorageServer::getFileType(const QFileInfo &fileInfo)
{
    if (fileInfo.isDir()) {
        return "dir";
    } else if (fileInfo.isFile()) {
        QString suffix = fileInfo.suffix();
        return suffix.isEmpty() ? "unknown" : suffix;
    }
    return "unknown";
}

QString RogalunaStorageServer::formatFileSize(qint64 size) const
{
    const qint64 KB = 1024;
    const qint64 MB = 1024 * KB;
    const qint64 GB = 1024 * MB;

    if (size >= GB) {
        return QString::number(static_cast<double>(size) / GB, 'f', 2) + " GB";
    } else if (size >= MB) {
        return QString::number(static_cast<double>(size) / MB, 'f', 2) + " MB";
    } else if (size >= KB) {
        return QString::number(static_cast<double>(size) / KB, 'f', 2) + " KB";
    } else {
        return QString::number(size) + " bytes";
    }
}
