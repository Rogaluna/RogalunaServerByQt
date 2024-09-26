#ifndef DEFAULTOPTIONSHANDLER_H
#define DEFAULTOPTIONSHANDLER_H

#include <QHttpServerResponse>


class DefaultOptionsHandler
{
public:
    DefaultOptionsHandler() = default;

    static QHttpServerResponse handleRequest(QUrl arg, const QHttpServerRequest &request);
};

#endif // DEFAULTOPTIONSHANDLER_H
