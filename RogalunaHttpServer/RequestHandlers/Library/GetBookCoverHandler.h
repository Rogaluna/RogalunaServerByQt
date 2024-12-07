#ifndef GETBOOKCOVERHANDLER_H
#define GETBOOKCOVERHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetBookCoverHandler
{
public:
    GetBookCoverHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETBOOKCOVERHANDLER_H
