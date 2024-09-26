#ifndef GETCHAPTERCONTENTHANDLER_H
#define GETCHAPTERCONTENTHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetChapterContentHandler
{
public:
    GetChapterContentHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETCHAPTERCONTENTHANDLER_H
