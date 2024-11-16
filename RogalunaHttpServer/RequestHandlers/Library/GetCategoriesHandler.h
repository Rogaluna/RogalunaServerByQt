#ifndef GETCATEGORIESHANDLER_H
#define GETCATEGORIESHANDLER_H

#include <QHttpServerResponse>


class GetCategoriesHandler
{
public:
    GetCategoriesHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETCATEGORIESHANDLER_H
