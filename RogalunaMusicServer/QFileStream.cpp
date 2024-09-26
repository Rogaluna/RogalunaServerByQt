#include "QFileStream.h"


QFileStream::QFileStream(QFile &file) : qfile(file)
{
    if (!qfile.isOpen()) {
        qfile.open(QIODevice::ReadOnly);
    }
}

TagLib::FileName QFileStream::name() const
{
    return TagLib::FileName(qfile.fileName().toUtf8().constData());
}

TagLib::ByteVector QFileStream::readBlock(size_t length)
{
    QByteArray data = qfile.read(length);
    return TagLib::ByteVector(data.data(), data.size());
}

void QFileStream::writeBlock(const TagLib::ByteVector &data)
{
    if (qfile.isWritable()) {
        qfile.write(data.data(), data.size());
    } else {
        // 如果文件是只读的，可以尝试重新以读写方式打开
        QString fileName = qfile.fileName();
        qfile.close();
        if (qfile.open(QIODevice::ReadWrite)) {
            qfile.write(data.data(), data.size());
        } else {
            // 无法写入文件，可能需要处理错误
        }
    }
}

void QFileStream::insert(const TagLib::ByteVector &data, TagLib::offset_t start, size_t replace)
{
    // 插入数据需要在文件中间进行操作，这在 QFile 中不是直接支持的
    // 需要读取文件内容，修改后再写回

    QByteArray originalData = qfile.readAll();
    QByteArray newData;
    newData.append(originalData.left(start));
    newData.append(data.data(), data.size());
    newData.append(originalData.mid(start + replace));

    qfile.seek(0);
    qfile.resize(0);
    qfile.write(newData);
}

void QFileStream::removeBlock(TagLib::offset_t start, size_t length)
{
    // 与 insert 类似，需要修改文件内容
    QByteArray originalData = qfile.readAll();
    QByteArray newData;
    newData.append(originalData.left(start));
    newData.append(originalData.mid(start + length));

    qfile.seek(0);
    qfile.resize(0);
    qfile.write(newData);
}

bool QFileStream::readOnly() const
{
    return !(qfile.isWritable());
}

bool QFileStream::isOpen() const
{
    return qfile.isOpen();
}

void QFileStream::seek(TagLib::offset_t offset, Position p)
{
    if (p == Beginning) {
        qfile.seek(offset);
    } else if (p == Current) {
        qfile.seek(qfile.pos() + offset);
    } else if (p == End) {
        qfile.seek(qfile.size() + offset);
    }
}

TagLib::offset_t QFileStream::tell() const
{
    return qfile.pos();
}

TagLib::offset_t QFileStream::length()
{
    return qfile.size();
}

void QFileStream::truncate(TagLib::offset_t length)
{
    qfile.resize(length);
}

void QFileStream::clear()
{
    // 清除错误和 EOF 标志，QFile 没有直接的方法，需要重新打开
    QString fileName = qfile.fileName();
    QIODevice::OpenMode mode = qfile.openMode();
    qfile.close();
    qfile.open(mode);
}
