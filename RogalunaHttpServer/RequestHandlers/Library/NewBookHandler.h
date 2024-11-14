#ifndef NEWBOOKHANDLER_H
#define NEWBOOKHANDLER_H

#include <QHttpServerResponse>


class NewBookHandler
{
public:
    NewBookHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // NEWBOOKHANDLER_H
