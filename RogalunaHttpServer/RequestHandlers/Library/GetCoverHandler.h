#ifndef GETCOVERHANDLER_H
#define GETCOVERHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetCoverHandler
{
public:
    GetCoverHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETCOVERHANDLER_H
