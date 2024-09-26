#ifndef GETBOOKCATEGORIES_H
#define GETBOOKCATEGORIES_H

#include <QHttpServerResponse>


class GetBookCategories
{
public:
    GetBookCategories() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // GETBOOKCATEGORIES_H
