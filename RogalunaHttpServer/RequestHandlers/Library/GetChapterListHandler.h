#ifndef GETCHAPTERLISTHANDLER_H
#define GETCHAPTERLISTHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetChapterListHandler
{
public:
    GetChapterListHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETCHAPTERLISTHANDLER_H
