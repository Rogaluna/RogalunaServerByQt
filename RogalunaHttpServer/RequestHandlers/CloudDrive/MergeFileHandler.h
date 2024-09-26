#ifndef MERGEFILEHANDLER_H
#define MERGEFILEHANDLER_H

#include <QHttpServerResponse>

namespace CloudDrive {

class MergeFileHandler
{
public:
    MergeFileHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // MERGEFILEHANDLER_H
