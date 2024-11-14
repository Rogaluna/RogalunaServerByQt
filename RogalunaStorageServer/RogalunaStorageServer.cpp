#include "RogalunaStorageServer.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>

RogalunaStorageServer::RogalunaStorageServer(const QString &rootDir, const QString &tempDir, const QString &tempFilePrefix)
    : root(rootDir)
    , temp(tempDir)
    , tempFilePrefix(tempFilePrefix)
{
    if (!root.endsWith(QDir::separator()))
    {
        root += QDir::separator();
    }

    QDir targetRootDir(root);
    if (!targetRootDir.exists()) {
        targetRootDir.mkpath(".");  // 创建根文件夹
    }

    if (!temp.endsWith(QDir::separator()))
    {
        temp += QDir::separator();
    }

    QDir targetTempDir(temp);
    if (!targetTempDir.exists()) {
        targetTempDir.mkpath(".");  // 创建临时文件夹
    }
}

QString RogalunaStorageServer::absoluteFilePath(const QString &relativeFilePath) const
{
    QString result = root + relativeFilePath;

    // 如果 result 以 '/' 结尾，去掉它
    if (result.endsWith(QDir::separator())) {
        result.chop(1);
    }
    return result;
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

bool RogalunaStorageServer::writeFile(const QString &path, const QByteArray &data, qint64 bufferSize)
{
    QFile file(path);

    // 检查并创建路径中缺失的目录
    QDir dir = QFileInfo(file).absoluteDir();
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Failed to create directory for file:" << path;
        return false;
    }

    // 打开文件，使用写入模式和覆盖模式，如果文件不存在则创建
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Failed to open file:" << path;
        return false;
    }

    qint64 bytesWritten = 0;
    qint64 totalBytes = data.size();

    // 按照指定的缓冲区大小逐块写入数据
    while (bytesWritten < totalBytes) {
        qint64 chunkSize = qMin(bufferSize, totalBytes - bytesWritten);
        qint64 written = file.write(data.mid(bytesWritten, chunkSize));
        if (written == -1) {
            qWarning() << "Error writing to file:" << path;
            return false;  // 写入失败
        }
        bytesWritten += written;
    }

    file.close();
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

FileReadResult RogalunaStorageServer::readFile(QFile &file, qint64 offset, qint64 bufferSize) {
    FileReadResult result;

    if (!file.open(QIODevice::ReadOnly)) {
        result.success = false;
        result.error = "File is not open";
        return result;
    }

    if (!file.seek(offset)) {
        result.success = false;
        result.error = "Failed to seek to position";
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

FileReadResult RogalunaStorageServer::readFile(const QString &path, qint64 offset, qint64 bufferSize)
{
    QFile file(path);

    FileReadResult result = readFile(file, offset, bufferSize);

    file.close();

    return result;
}

QVector<FileInfoStruct> RogalunaStorageServer::listFiles(const QString &relativeDirPath, bool includeDirs)
{
    QString dirPath = absoluteFilePath(relativeDirPath);
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(dirPath);  // 创建目标文件夹
    }

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

bool RogalunaStorageServer::writeTempFile(const QString &tempFileDirName, int chunkIndex, const QByteArray &chunkData, const QString &chunkMd5)
{
    QString tempDirPath = getTempPath() + tempFileDirName;
    QDir tempDir(tempDirPath);
    if (!tempDir.exists()) {
        tempDir.mkpath(tempDirPath);  // 创建临时文件夹
    }

    // 每个文件块的临时文件路径
    QString tempFileChunkPath = tempDirPath + "/" + tempFilePrefix + QString::number(chunkIndex);

    // 将块保存到临时文件
    QFile tempFile(tempFileChunkPath);
    if (!tempFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    tempFile.write(chunkData);

    // 如果有校验码，那么进行校验，如果没有，直接保存
    if (!chunkMd5.isEmpty()) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(&tempFile);
        QString calculatedMd5 = hash.result().toHex();

        // 验证 MD5 校验是否匹配，如果不匹配，则删除文件
        if (calculatedMd5 != chunkMd5) {
            tempFile.close(); // 关闭临时文件
            tempFile.remove(); // 删除文件块

            return false;
        }
    }

    tempFile.close();

    return true;
}

bool RogalunaStorageServer::mergeTempFile(const QString &tempFileDirName, int totalChunks, const QString &targetDir, const QString &mergeFileName)
{
    // 获取临时目录路径
    QString tempDirPath = getTempPath() + tempFileDirName;
    QDir tempDir(tempDirPath);

    if (!tempDir.exists()) {
        return false;
    }

    QString targetFilePath = absoluteFilePath(targetDir) + "/" + mergeFileName;
    // 打开最终文件用于写入
    QFile finalFile(targetFilePath);
    if (!finalFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    // 按顺序合并所有块
    for (qint64 i = 0; i < totalChunks; ++i) {
        QString chunkFilePath = tempDirPath + "/" + tempFilePrefix + QString::number(i);
        QFile chunkFile(chunkFilePath);
        if (!chunkFile.open(QIODevice::ReadOnly)) {
            finalFile.close();
            return false;
        }

        // 将块数据写入最终文件
        finalFile.write(chunkFile.readAll());
        chunkFile.close();
    }

    // 合并完成，关闭文件
    finalFile.close();

    // 删除临时文件夹及其中的所有块文件
    tempDir.removeRecursively();

    return true;
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
