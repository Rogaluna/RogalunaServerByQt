#ifndef CONTENTDAO_H
#define CONTENTDAO_H

#include <BaseDAO.h>
#include <Interface/CloudDrive/IContentDAO.h>

class ROGALUNADATABASESERVER_EXPORT ContentDAO : public BaseDAO, public IContentDAO
{
public:
    explicit ContentDAO(QSqlDatabase& db, const QString& schema = "cloud_drive")
        : BaseDAO(db, schema, "content") {}

    // 实现 IContentDAO 接口的方法
    bool insertContent(const QString& contentMd5, qint64 size) override;

    QString getContentPath(const QString& contentMd5) override;

    bool deleteContent(const QString& contentMd5) override;
};

#endif // CONTENTDAO_H
