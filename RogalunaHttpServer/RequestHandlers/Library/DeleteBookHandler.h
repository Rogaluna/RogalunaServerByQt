#ifndef DELETEBOOKHANDLER_H
#define DELETEBOOKHANDLER_H

#include <QHttpServerResponse>


class DeleteBookHandler
{
public:
    DeleteBookHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // DELETEBOOKHANDLER_H
