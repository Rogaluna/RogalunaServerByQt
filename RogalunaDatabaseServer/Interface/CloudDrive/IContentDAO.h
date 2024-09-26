#ifndef ICONTENTDAO_H
#define ICONTENTDAO_H

#include <QString>

namespace CloudDrive {

class IContentDAO
{
public:
    virtual ~IContentDAO() {}

    virtual bool insertContent(const QString& contentMd5, qint64 size) = 0;
    virtual bool deleteContent(const QString& contentMd5) = 0;
};

}

#endif // ICONTENTDAO_H
