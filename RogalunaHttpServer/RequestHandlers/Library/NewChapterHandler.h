#ifndef NEWCHAPTERHANDLER_H
#define NEWCHAPTERHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class NewChapterHandler
{
public:
    NewChapterHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // NEWCHAPTERHANDLER_H
