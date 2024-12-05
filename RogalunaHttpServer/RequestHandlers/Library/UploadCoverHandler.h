#ifndef UPLOADCOVERHANDLER_H
#define UPLOADCOVERHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class UploadCoverHandler
{
public:
    UploadCoverHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // UPLOADCOVERHANDLER_H
