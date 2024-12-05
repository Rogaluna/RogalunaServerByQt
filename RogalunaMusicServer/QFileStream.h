#ifndef QFILESTREAM_H
#define QFILESTREAM_H

#include <taglib/toolkit/tiostream.h>

#include <QFile>


class QFileStream : public TagLib::IOStream
{
public:
    QFileStream(QFile &file);

    virtual ~QFileStream() {}

    virtual TagLib::FileName name() const override;

    virtual TagLib::ByteVector readBlock(size_t length) override;

    virtual void writeBlock(const TagLib::ByteVector &data) override;

    virtual void insert(const TagLib::ByteVector &data, long long start, size_t replace) override;

    virtual void removeBlock(long long start, size_t length) override;

    virtual bool readOnly() const override;

    virtual bool isOpen() const override;

    virtual void seek(long long offset, Position p = Beginning) override;

    virtual long long tell() const override;

    virtual long long length() override;

    virtual void truncate(long long length) override;

    void clear() override;

private:
    QFile &qfile;
};

#endif // QFILESTREAM_H
