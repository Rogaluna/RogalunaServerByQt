#ifndef UPDATEBOOKINFOHANDLER_H
#define UPDATEBOOKINFOHANDLER_H

#include <QHttpServerResponse>


class UpdateBookInfoHandler
{
public:
    UpdateBookInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // UPDATEBOOKINFOHANDLER_H
