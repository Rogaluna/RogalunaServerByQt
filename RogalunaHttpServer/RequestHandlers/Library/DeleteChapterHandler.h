#ifndef DELETECHAPTERHANDLER_H
#define DELETECHAPTERHANDLER_H

#include <QHttpServerResponse>


class DeleteChapterHandler
{
public:
    DeleteChapterHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // DELETECHAPTERHANDLER_H
