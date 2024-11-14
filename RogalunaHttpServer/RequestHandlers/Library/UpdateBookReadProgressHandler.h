#ifndef UPDATEBOOKREADPROGRESSHANDLER_H
#define UPDATEBOOKREADPROGRESSHANDLER_H

#include <QHttpServerResponse>


class UpdateBookReadProgressHandler
{
public:
    UpdateBookReadProgressHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // UPDATEBOOKREADPROGRESSHANDLER_H
