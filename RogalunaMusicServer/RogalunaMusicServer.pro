QT -= gui

TEMPLATE = lib
DEFINES += ROGALUNAMUSICSERVER_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Cover/FLACAlbumCoverHandler.cpp \
    Cover/MP3AlbumCoverHandler.cpp \
    QFileStream.cpp \
    RogalunaMusicServer.cpp

HEADERS += \
    AlbumCoverHandler.h \
    Cover/FLACAlbumCoverHandler.h \
    Cover/MP3AlbumCoverHandler.h \
    QFileStream.h \
    RogalunaMusicServer_global.h \
    RogalunaMusicServer.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaStorageServer/release/ -lRogalunaStorageServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaStorageServer/debug/ -lRogalunaStorageServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaStorageServer/ -lRogalunaStorageServer

INCLUDEPATH += $$PWD/../RogalunaStorageServer
DEPENDPATH += $$PWD/../RogalunaStorageServer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaDatabaseServer/release/ -lRogalunaDatabaseServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaDatabaseServer/debug/ -lRogalunaDatabaseServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaDatabaseServer/ -lRogalunaDatabaseServer

INCLUDEPATH += $$PWD/../RogalunaDatabaseServer
DEPENDPATH += $$PWD/../RogalunaDatabaseServer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QTagLib/release/ -lQTagLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QTagLib/debug/ -lQTagLib
else:unix:!macx: LIBS += -L$$OUT_PWD/../QTagLib/ -lQTagLib

INCLUDEPATH += $$PWD/../QTagLib
DEPENDPATH += $$PWD/../QTagLib
