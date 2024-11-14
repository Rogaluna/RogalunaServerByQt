#ifndef GETCHAPTERLISTHANDLER_H
#define GETCHAPTERLISTHANDLER_H

#include <QHttpServerResponse>


class GetChapterListHandler
{
public:
    GetChapterListHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETCHAPTERLISTHANDLER_H
