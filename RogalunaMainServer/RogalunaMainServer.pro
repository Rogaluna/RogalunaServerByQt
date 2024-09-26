QT = core
QT += network
QT += httpserver

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ConfigFile.h

SOURCES += \
        ConfigFile.cpp \
        Main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaDatabaseServer/release/ -lRogalunaDatabaseServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaDatabaseServer/debug/ -lRogalunaDatabaseServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaDatabaseServer/ -lRogalunaDatabaseServer

INCLUDEPATH += $$PWD/../RogalunaDatabaseServer
DEPENDPATH += $$PWD/../RogalunaDatabaseServer


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaStorageServer/release/ -lRogalunaStorageServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaStorageServer/debug/ -lRogalunaStorageServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaStorageServer/ -lRogalunaStorageServer

INCLUDEPATH += $$PWD/../RogalunaStorageServer
DEPENDPATH += $$PWD/../RogalunaStorageServer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaHttpServer/release/ -lRogalunaHttpServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaHttpServer/debug/ -lRogalunaHttpServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaHttpServer/ -lRogalunaHttpServer

INCLUDEPATH += $$PWD/../RogalunaHttpServer
DEPENDPATH += $$PWD/../RogalunaHttpServer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QJsonWebToken/release/ -lQJsonWebToken
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QJsonWebToken/debug/ -lQJsonWebToken
else:unix:!macx: LIBS += -L$$OUT_PWD/../QJsonWebToken/ -lQJsonWebToken

INCLUDEPATH += $$PWD/../QJsonWebToken
DEPENDPATH += $$PWD/../QJsonWebToken

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaCloudDriveServer/release/ -lRogalunaCloudDriveServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaCloudDriveServer/debug/ -lRogalunaCloudDriveServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaCloudDriveServer/ -lRogalunaCloudDriveServer

INCLUDEPATH += $$PWD/../RogalunaCloudDriveServer
DEPENDPATH += $$PWD/../RogalunaCloudDriveServer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaLibraryServer/release/ -lRogalunaLibraryServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaLibraryServer/debug/ -lRogalunaLibraryServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaLibraryServer/ -lRogalunaLibraryServer

INCLUDEPATH += $$PWD/../RogalunaLibraryServer
DEPENDPATH += $$PWD/../RogalunaLibraryServer
