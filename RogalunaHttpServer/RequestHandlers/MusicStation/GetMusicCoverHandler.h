#ifndef GETMUSICCOVERHANDLER_H
#define GETMUSICCOVERHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {

class GetMusicCoverHandler
{
public:
    GetMusicCoverHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETMUSICCOVERHANDLER_H
