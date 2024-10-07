#ifndef MERGEFILEHANDLER_H
#define MERGEFILEHANDLER_H

#include <QHttpServerResponse>

class MergeFileHandler
{
public:
    MergeFileHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

#endif // MERGEFILEHANDLER_H
