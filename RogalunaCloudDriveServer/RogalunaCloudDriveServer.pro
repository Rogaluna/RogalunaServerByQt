QT -= gui

TEMPLATE = lib
DEFINES += ROGALUNACLOUDDRIVESERVER_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RogalunaCloudDriveServer.cpp

HEADERS += \
    RogalunaCloudDriveServer_global.h \
    RogalunaCloudDriveServer.h

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
