#ifndef ROGALUNAMUSICSERVER_GLOBAL_H
#define ROGALUNAMUSICSERVER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ROGALUNAMUSICSERVER_LIBRARY)
#define ROGALUNAMUSICSERVER_EXPORT Q_DECL_EXPORT
#else
#define ROGALUNAMUSICSERVER_EXPORT Q_DECL_IMPORT
#endif

#endif // ROGALUNAMUSICSERVER_GLOBAL_H
