#ifndef UPLOADLIBRARYRESOURCEHANDLER_H
#define UPLOADLIBRARYRESOURCEHANDLER_H

#include <QHttpServerResponse>


class UploadLibraryResourceHandler
{
public:
    UploadLibraryResourceHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // UPLOADLIBRARYRESOURCEHANDLER_H
