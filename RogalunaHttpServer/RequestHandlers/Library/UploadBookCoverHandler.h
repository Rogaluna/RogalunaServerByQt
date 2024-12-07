#ifndef UPLOADBOOKCOVERHANDLER_H
#define UPLOADBOOKCOVERHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class UploadBookCoverHandler
{
public:
    UploadBookCoverHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // UPLOADBOOKCOVERHANDLER_H
