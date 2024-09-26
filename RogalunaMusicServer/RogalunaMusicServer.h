#ifndef ROGALUNAMUSICSERVER_H
#define ROGALUNAMUSICSERVER_H

#include "RogalunaMusicServer_global.h"

#include <QObject>

class RogalunaStorageServer;

class ROGALUNAMUSICSERVER_EXPORT RogalunaMusicServer
{
public:
    RogalunaMusicServer();

// public:
//     bool addMusic();


private:
    RogalunaStorageServer* storageServer;
};

#endif // ROGALUNAMUSICSERVER_H
