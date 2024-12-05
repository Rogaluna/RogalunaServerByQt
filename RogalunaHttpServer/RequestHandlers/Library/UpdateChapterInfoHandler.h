#ifndef UPDATECHAPTERINFOHANDLER_H
#define UPDATECHAPTERINFOHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class UpdateChapterInfoHandler
{
public:
    UpdateChapterInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // UPDATECHAPTERINFOHANDLER_H
