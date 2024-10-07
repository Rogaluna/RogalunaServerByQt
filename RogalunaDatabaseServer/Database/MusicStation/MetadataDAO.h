#ifndef METADATADAO_H
#define METADATADAO_H

#include <BaseDAO.h>
#include <Interface/MusicStation/IMetaDataDAO.h>

class MetadataDAO: public BaseDAO, public IMetadataDAO
{
public:
    MetadataDAO();

public:

    std::optional<FFileMetadata> getMetadataByUid(const QString& uid) override;
    std::optional<QVector<FFileMetadata>> getRandomMetadata() override;
};

#endif // METADATADAO_H
