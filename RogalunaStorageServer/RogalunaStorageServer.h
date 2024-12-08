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

/**
 * @class RogalunaStorageServer
 * @brief 一个用于管理文件操作的存储服务器类，包括文件的读取、写入、删除和合并操作。
 */
class ROGALUNASTORAGESERVER_EXPORT RogalunaStorageServer
{
public:
    /**
     * @brief 构造一个 RogalunaStorageServer 实例。
     * @param rootDir 文件存储的根目录。
     * @param tempDir 临时文件存储的目录。
     * @param tempFilePrefix 临时文件的命名前缀。
     */
    RogalunaStorageServer(const QString &rootDir, const QString &tempDir, qint64 bufferSize);

    // 删除拷贝构造函数和赋值操作符，防止实例被拷贝
    RogalunaStorageServer(const RogalunaStorageServer&) = delete;
    RogalunaStorageServer& operator=(const RogalunaStorageServer&) = delete;

public:
    /**
     * @brief 获取文件相对程序的路径。（旧名没有变动）
     * @param <QString> relativeFilePath 文件的相对路径。
     * @param <QString> 基底检索路径，默认是空，即 root 指向的路径。
     * @return <QString> 文件相对程序的路径。
     */
    QString absoluteFilePath(const QString &relativeFilePath, const QString &basePath = "") const;

    /**
     * @brief 写入数据到指定的 QFile 文件对象。
     * @param file 要写入的 QFile 对象。
     * @param data 要写入的数据。
     * @param bufferSize 每次写入的缓冲区大小，默认为 4096 字节。
     * @return 如果写入成功返回 true，否则返回 false。
     */
    bool writeFile(QFile &file, const QByteArray &data, const QString &expectedMd5 = "", qint64 buffer = 4096);

    /**
     * @brief 写入数据到指定路径的文件。如果文件已存在，将覆盖；如果不存在，将创建文件。
     * @param path 要写入的文件路径。
     * @param data 要写入的数据。
     * @param bufferSize 每次写入的缓冲区大小，默认为 4096 字节。
     * @return 如果写入成功返回 true，否则返回 false。
     */
    bool writeFile(const QString &path, const QByteArray &data, const QString &expectedMd5 = "", qint64 buffer = 4096);

    /**
     * @brief 删除指定路径的文件或文件夹。
     * @param path 要删除的资源路径。
     * @return 如果删除成功返回 true，否则返回 false。
     */
    bool deleteFile(const QString &path);
    bool deleteFile(QFile &file);
    bool deleteFile(QDir &dir);

    /**
     * @brief 读取 QFile 对象中的数据。
     * @param file 要读取的 QFile 对象。
     * @param offset 读取的字节偏移量。
     * @param bufferSize 每次读取的缓冲区大小，默认为 4096 字节。
     * @return 返回包含读取结果的 FileReadResult 对象。
     */
    FileReadResult readFile(QFile &file, qint64 offset, qint64 buffer = 4096);

    /**
     * @brief 读取指定路径的文件。
     * @param path 要读取的文件路径。
     * @param offset 读取的字节偏移量。
     * @param bufferSize 每次读取的缓冲区大小，默认为 4096 字节。
     * @return 返回包含读取结果的 FileReadResult 对象。
     */
    FileReadResult readFile(const QString &path, qint64 offset, qint64 buffer = 4096);

    /**
     * @brief 重命名指定路径的文件。
     * @param path 要读取的文件路径。
     * @param newName 新的名称。
     * @return 返回是否成功重命名。
     */
    bool renameFile(const QString &path, const QString &newName);

    /**
     * @brief 列出指定目录中的文件和文件夹。
     * @param directoryPath 要列出的目录路径。
     * @param includeDirs 是否包含目录，默认为 true。
     * @return 返回包含文件信息的 QVector。
     */
    QVector<FileInfoStruct> listFiles(const QString &directoryPath, bool includeDirs = true);

    /**
     * @brief 获取临时文件夹的路径。
     * @return 临时文件夹路径的 QString。
     */
    QString getTempPath() const { return temp; };

    /**
     * @brief 合并临时文件夹中的文件块到目标目录。
     * @param tempFileDirName 临时文件目录名称。
     * @param totalChunks 总块数。
     * @param targetDir 目标目录（相对于存储根目录）。
     * @param mergeFileName 合并后的文件名。
     * @return 如果合并成功返回 true，否则返回 false。
     */
    bool mergeTempFile(const QString &tempFileDirName, int totalChunks, const QString &targetDir, const QString &mergeFileName);

    // 清理临时文件夹

private:
    /**
     * @brief 获取文件的类型。
     * @param fileInfo 文件的 QFileInfo 对象。
     * @return 文件类型的 QString。
     */
    QString getFileType(const QFileInfo &fileInfo);

    /**
     * @brief 格式化文件大小为易读的字符串。
     * @param size 文件大小（字节）。
     * @return 格式化后的文件大小字符串。
     */
    QString formatFileSize(qint64 size) const;

public:
    QString root;                    ///< 文件存储的根目录。
    QString temp;                    ///< 临时文件存储的目录。

private:
    qint64 bufferSize;               ///< 每次写入的缓冲大小。
};

#endif // ROGALUNASTORAGESERVER_H
