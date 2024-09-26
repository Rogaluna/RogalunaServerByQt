#ifndef UPDATECHAPTERCONTENTHANDLER_H
#define UPDATECHAPTERCONTENTHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class UpdateChapterContentHandler
{
public:
    UpdateChapterContentHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // UPDATECHAPTERCONTENTHANDLER_H
