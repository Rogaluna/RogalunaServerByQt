#ifndef MODIFYACCOUNTINFOHANDLER_H
#define MODIFYACCOUNTINFOHANDLER_H

#include <QHttpServerResponse>

namespace Account {


class ModifyAccountInfoHandler
{
public:
    ModifyAccountInfoHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // MODIFYACCOUNTINFOHANDLER_H
