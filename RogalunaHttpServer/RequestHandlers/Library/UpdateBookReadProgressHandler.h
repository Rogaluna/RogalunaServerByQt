#ifndef UPDATEBOOKREADPROGRESSHANDLER_H
#define UPDATEBOOKREADPROGRESSHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class UpdateBookReadProgressHandler
{
public:
    UpdateBookReadProgressHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // UPDATEBOOKREADPROGRESSHANDLER_H
