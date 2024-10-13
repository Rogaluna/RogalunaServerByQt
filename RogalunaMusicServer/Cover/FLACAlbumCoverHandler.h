#ifndef FLACALBUMCOVERHANDLER_H
#define FLACALBUMCOVERHANDLER_H

#include "AlbumCoverHandler.h"

class FLACAlbumCoverHandler : public AlbumCoverHandler
{
public:
    FLACAlbumCoverHandler() = default;
    QByteArray getAlbumCover(QFile &file) override;
};

#endif // FLACALBUMCOVERHANDLER_H
