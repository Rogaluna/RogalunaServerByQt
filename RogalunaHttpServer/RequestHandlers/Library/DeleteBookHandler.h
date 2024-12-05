#ifndef DELETEBOOKHANDLER_H
#define DELETEBOOKHANDLER_H

#include <QHttpServerResponse>

namespace Library {

class DeleteBookHandler
{
public:
    DeleteBookHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // DELETEBOOKHANDLER_H
