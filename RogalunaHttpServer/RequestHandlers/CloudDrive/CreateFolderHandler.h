#ifndef CREATEFOLDERHANDLER_H
#define CREATEFOLDERHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {


class CreateFolderHandler
{
public:
    CreateFolderHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // CREATEFOLDERHANDLER_H
