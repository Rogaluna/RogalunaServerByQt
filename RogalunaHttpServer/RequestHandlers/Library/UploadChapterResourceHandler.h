#ifndef UPLOADCHAPTERRESOURCEHANDLER_H
#define UPLOADCHAPTERRESOURCEHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class UploadChapterResourceHandler
{
public:
    UploadChapterResourceHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // UPLOADCHAPTERRESOURCEHANDLER_H
