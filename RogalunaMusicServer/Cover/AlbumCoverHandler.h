#ifndef ALBUMCOVERHANDLER_H
#define ALBUMCOVERHANDLER_H

#include <QFile>


class AlbumCoverHandler
{
public:
    virtual ~AlbumCoverHandler() = default;
    virtual QByteArray getAlbumCover(QFile &file) = 0;
};

#endif // ALBUMCOVERHANDLER_H
