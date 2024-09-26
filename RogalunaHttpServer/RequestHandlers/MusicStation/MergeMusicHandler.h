#ifndef MERGEMUSICHANDLER_H
#define MERGEMUSICHANDLER_H

#include <QHttpServerResponse>

namespace MusicStation {


class MergeMusicHandler
{
public:
    MergeMusicHandler() = default;

    static QHttpServerResponse handleRequest(const QHttpServerRequest &request);
};

}

#endif // MERGEMUSICHANDLER_H
