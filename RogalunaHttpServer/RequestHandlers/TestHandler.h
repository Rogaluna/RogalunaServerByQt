#ifndef TESTHANDLER_H
#define TESTHANDLER_H

#include <QHttpServerResponse>

class TestHandler
{
public:
    TestHandler() = default;

public:
    static QHttpServerResponse handleRequest(QString testArg, const QHttpServerRequest &request) {
        Q_UNUSED(request);

        return QHttpServerResponse(QString("Test Handler, arg value: %1").arg(testArg));
    }
};

#endif // TESTHANDLER_H
