#ifndef MP3ALBUMCOVERHANDLER_H
#define MP3ALBUMCOVERHANDLER_H

#include "AlbumCoverHandler.h"

class MP3AlbumCoverHandler : public AlbumCoverHandler
{
public:
    MP3AlbumCoverHandler() = default;
    QByteArray extractAlbumCover(QFile &file) override;
};

#endif // MP3ALBUMCOVERHANDLER_H
