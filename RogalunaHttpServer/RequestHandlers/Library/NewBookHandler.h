#ifndef NEWBOOKHANDLER_H
#define NEWBOOKHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class NewBookHandler
{
public:
    NewBookHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // NEWBOOKHANDLER_H
