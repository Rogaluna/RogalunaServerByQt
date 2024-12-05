#ifndef REGISTERHANDLER_H
#define REGISTERHANDLER_H

#include <QHttpServerResponse>

namespace Account {

class RegisterHandler
{
public:
    RegisterHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // REGISTERHANDLER_H
