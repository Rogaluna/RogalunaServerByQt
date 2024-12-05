#ifndef GETLIBRARYRESOURCEHANDLER_H
#define GETLIBRARYRESOURCEHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetLibraryResourceHandler
{
public:
    GetLibraryResourceHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETLIBRARYRESOURCEHANDLER_H
