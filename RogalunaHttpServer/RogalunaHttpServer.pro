QT -= gui
QT += network
QT += httpserver

TEMPLATE = lib
DEFINES += ROGALUNAHTTPSERVER_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RequestHandlers/Account/LoginHandler.cpp \
    RequestHandlers/Account/RegisterHandler.cpp \
    RequestHandlers/DefaultOptionsHandler.cpp \
    RequestHandlers/FileStorage/CreateFolderHandler.cpp \
    RequestHandlers/FileStorage/FetchFileDirectLinkHandler.cpp \
    RequestHandlers/FileStorage/GetFileHandler.cpp \
    RequestHandlers/FileStorage/GetFileListHandler.cpp \
    RequestHandlers/FileStorage/MergeFileHandler.cpp \
    RequestHandlers/FileStorage/PostFileHandler.cpp \
    RequestHandlers/GetWebFileHandler.cpp \
    RequestHandlers/Library/GetBookCategories.cpp \
    RogalunaHttpServer.cpp

HEADERS += \
    Macro/RequestBodyParser.h \
    Macro/TokenGV.h \
    RequestHandlers/Account/LoginHandler.h \
    RequestHandlers/Account/RegisterHandler.h \
    RequestHandlers/DefaultOptionsHandler.h \
    RequestHandlers/FileStorage/CreateFolderHandler.h \
    RequestHandlers/FileStorage/FetchFileDirectLinkHandler.h \
    RequestHandlers/FileStorage/GetFileHandler.h \
    RequestHandlers/FileStorage/GetFileListHandler.h \
    RequestHandlers/FileStorage/MergeFileHandler.h \
    RequestHandlers/FileStorage/PostFileHandler.h \
    RequestHandlers/GetWebFileHandler.h \
    RequestHandlers/Library/GetBookCategories.h \
    RequestHandlers/TestHandler.h \
    RogalunaHttpConfig.h \
    RogalunaHttpServer.h \
    RogalunaHttpServer_Global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/../thirdparty/bcrypt/include
LIBS += -L$$PWD/../thirdparty/bcrypt/lib -lbcrypt

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


INCLUDEPATH += $$PWD/../thirdparty/bcrypt/include
LIBS += -L$$PWD/../thirdparty/bcrypt/lib -lbcrypt

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

