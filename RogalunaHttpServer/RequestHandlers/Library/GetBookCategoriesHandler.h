#ifndef GETBOOKCATEGORIESHANDLER_H
#define GETBOOKCATEGORIESHANDLER_H

#include <QHttpServerResponse>

namespace Library {


class GetBookCategoriesHandler
{
public:
    GetBookCategoriesHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // GETBOOKCATEGORIESHANDLER_H
