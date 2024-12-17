#include "RogalunaStorageServer.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>

RogalunaStorageServer::RogalunaStorageServer(const QString &rootDir, const QString &tempDir, qint64 bufferSize)
    : root(rootDir)
    , temp(tempDir)
    , bufferSize(bufferSize)
{
    if (!root.endsWith(QDir::separator()))
    {
        root += QDir::separator();
    }

    QDir targetRootDir(root);
    if (!targetRootDir.exists()) {
        targetRootDir.mkpath(QDir::separator());  // 创建根文件夹
    }

    if (!temp.endsWith(QDir::separator()))
    {
        temp += QDir::separator();
    }

    QDir targetTempDir(temp);
    if (!targetTempDir.exists()) {
        targetTempDir.mkpath(QDir::separator());  // 创建临时文件夹
    }
}

QString RogalunaStorageServer::absoluteFilePath(const QString &relativeFilePath, const QString &basePath) const
{
    QString result = (basePath.isEmpty() ? root : basePath) + relativeFilePath;

    // 如果 result 以 '/' 结尾，去掉它
    if (result.endsWith(QDir::separator())) {
        result.chop(1);
    }
    return result;
}

bool RogalunaStorageServer::writeFile(QFile &file, const QByteArray &data, const QString &expectedMd5, qint64 buffer)
{
    if (buffer <= 0) {
        buffer = this->bufferSize;
    }

    if (!file.isOpen() || !(file.openMode() & QIODevice::WriteOnly)) {
        qWarning() << "File is not open or not in write mode";
        return false;
    }

    qint64 bytesWritten = 0;
    qint64 totalBytes = data.size();

    // 分块写入数据
    while (bytesWritten < totalBytes) {
        qint64 chunkSize = qMin(buffer, totalBytes - bytesWritten);
        qint64 written = file.write(data.mid(bytesWritten, chunkSize));
        if (written == -1) {
            qWarning() << "Error writing to file";
            return false;
        }
        bytesWritten += written;
    }

    // MD5 校验（如果提供了校验码）
    if (!expectedMd5.isEmpty()) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(data);
        QString calculatedMd5 = hash.result().toHex();

        if (calculatedMd5 != expectedMd5) {
            qWarning() << "MD5 mismatch for file";
            file.remove(); // 删除写入失败的文件
            return false;
        }
    }

    return true;
}

bool RogalunaStorageServer::writeFile(const QString &path, const QByteArray &data, const QString &expectedMd5, qint64 buffer)
{
    if (buffer <= 0) {
        buffer = this->bufferSize;
    }

    QFile file(path);

    // 检查并创建路径中的缺失目录
    QDir dir = QFileInfo(file).absoluteDir();
    if (!dir.exists() && !dir.mkpath(QDir::separator())) {
        qWarning() << "Failed to create directory for file:" << path;
        return false;
    }

    // 打开文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Failed to open file:" << path;
        return false;
    }

    // 调用 QFile 版本
    return writeFile(file, data, expectedMd5, buffer);
}

bool RogalunaStorageServer::deleteFile(const QString &path)
{
    QFileInfo fileInfo(path); // 获取路径信息

    if (!fileInfo.exists()) {
        qDebug() << "Path does not exist:" << path;
        return false;
    }

    if (fileInfo.isFile()) {
        // 如果是文件，调用 QFile 版本重载
        QFile file(path);
        return deleteFile(file);
    } else if (fileInfo.isDir()) {
        // 如果是目录，调用 QDir 版本重载
        QDir dir(path);
        return deleteFile(dir);
    }

    return false;
}

bool RogalunaStorageServer::deleteFile(QFile &file)
{
    if (file.remove()) {
        qDebug() << "File deleted successfully:" << file.fileName();
        return true;
    } else {
        qDebug() << "Failed to delete file:" << file.fileName();
        return false;
    }
}

bool RogalunaStorageServer::deleteFile(QDir &dir)
{
    if (dir.removeRecursively()) {
        qDebug() << "Directory deleted successfully:" << dir.absolutePath();
        return true;
    } else {
        qDebug() << "Failed to delete directory:" << dir.absolutePath();
        return false;
    }
}

FileReadResult RogalunaStorageServer::readFile(QFile &file, qint64 offset, qint64 buffer)
{
    if (buffer <= 0) {
        buffer = this->bufferSize;
    }

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
    result.data = file.read(buffer);
    result.success = !result.data.isEmpty();
    result.atEnd = file.atEnd();

    if (!result.success && !result.atEnd) {
        result.error = "Failed to read file";
    }

    return result;
}

FileReadResult RogalunaStorageServer::readFile(const QString &path, qint64 offset, qint64 buffer)
{
    if (buffer <= 0) {
        buffer = this->bufferSize;
    }

    QFile file(path);

    FileReadResult result = readFile(file, offset, buffer);

    file.close();

    return result;
}

bool RogalunaStorageServer::renameFile(const QString &path, const QString &newName)
{
    // 获取文件所在的目录和文件名
    QFileInfo fileInfo(path);
    QString dirPath = fileInfo.absolutePath();
    QString newFilePath = dirPath + QDir::separator() + newName;

    // 检查源文件是否存在
    if (!QFile::exists(path)) {
        qWarning() << "Source file does not exist:" << path;
        return false;
    }

    // 尝试重命名文件
    if (QFile::rename(path, newFilePath)) {
        qDebug() << "File renamed successfully from" << path << "to" << newFilePath;
        return true;
    } else {
        qWarning() << "Failed to rename file:" << QFile(path).errorString();
        return false;
    }
}

QVector<FileInfoStruct> RogalunaStorageServer::listFiles(const QString &relativeDirPath, bool includeDirs)
{
    QString dirPath = absoluteFilePath(relativeDirPath);
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(QDir::separator());  // 创建目标文件夹
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

bool RogalunaStorageServer::mergeTempFile(const QString &tempFileDirName, int totalChunks, const QString &targetDir, const QString &mergeFileName)
{
    // 获取临时目录路径
    QString tempDirPath = getTempPath() + tempFileDirName;
    QDir tempDir(tempDirPath);

    if (!tempDir.exists()) {
        return false;
    }

    QString targetFilePath = absoluteFilePath(targetDir) + QDir::separator() + mergeFileName;
    // 打开最终文件用于写入
    QFile finalFile(targetFilePath);
    if (!finalFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    // 按顺序合并所有块
    for (qint64 i = 0; i < totalChunks; ++i) {
        QString chunkFilePath = tempDirPath + QDir::separator() + QString::number(i);
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
