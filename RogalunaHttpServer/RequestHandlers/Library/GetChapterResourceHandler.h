#ifndef GETCHAPTERRESOURCEHANDLER_H
#define GETCHAPTERRESOURCEHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetChapterResourceHandler
{
public:
    GetChapterResourceHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETCHAPTERRESOURCEHANDLER_H
