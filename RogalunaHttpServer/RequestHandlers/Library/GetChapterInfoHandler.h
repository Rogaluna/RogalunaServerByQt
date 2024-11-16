#ifndef GETCHAPTERINFOHANDLER_H
#define GETCHAPTERINFOHANDLER_H

#include <QHttpServerResponse>


class GetChapterInfoHandler
{
public:
    GetChapterInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETCHAPTERINFOHANDLER_H
