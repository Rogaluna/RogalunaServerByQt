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
    RequestHandlers/CloudDrive/GetUidFromPathHandler.cpp \
    RequestHandlers/DefaultOptionsHandler.cpp \
    RequestHandlers/CloudDrive/CreateFolderHandler.cpp \
    RequestHandlers/CloudDrive/FetchFileDirectLinkHandler.cpp \
    RequestHandlers/CloudDrive/GetFileHandler.cpp \
    RequestHandlers/CloudDrive/GetFileListHandler.cpp \
    RequestHandlers/CloudDrive/GetParentFolderHandler.cpp \
    RequestHandlers/CloudDrive/MergeFileHandler.cpp \
    RequestHandlers/CloudDrive/PostFileHandler.cpp \
    RequestHandlers/GetWebFileHandler.cpp \
    RequestHandlers/Library/GetBookCategoriesHandler.cpp \
    RequestHandlers/MusicStation/GetMusicHandler.cpp \
    RequestHandlers/MusicStation/GetMusicListHandler.cpp \
    RequestHandlers/MusicStation/MergeMusicHandler.cpp \
    RequestHandlers/MusicStation/PostMusicHandler.cpp \
    RogalunaHttpServer.cpp

HEADERS += \
    Macro/RequestBodyParser.h \
    Macro/TokenGV.h \
    RequestHandlers/Account/LoginHandler.h \
    RequestHandlers/Account/RegisterHandler.h \
    RequestHandlers/CloudDrive/GetUidFromPathHandler.h \
    RequestHandlers/DefaultOptionsHandler.h \
    RequestHandlers/CloudDrive/CreateFolderHandler.h \
    RequestHandlers/CloudDrive/FetchFileDirectLinkHandler.h \
    RequestHandlers/CloudDrive/GetFileHandler.h \
    RequestHandlers/CloudDrive/GetFileListHandler.h \
    RequestHandlers/CloudDrive/GetParentFolderHandler.h \
    RequestHandlers/CloudDrive/MergeFileHandler.h \
    RequestHandlers/CloudDrive/PostFileHandler.h \
    RequestHandlers/GetWebFileHandler.h \
    RequestHandlers/Library/GetBookCategoriesHandler.h \
    RequestHandlers/MusicStation/GetMusicHandler.h \
    RequestHandlers/MusicStation/GetMusicListHandler.h \
    RequestHandlers/MusicStation/MergeMusicHandler.h \
    RequestHandlers/MusicStation/PostMusicHandler.h \
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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaMusicServer/release/ -lRogalunaMusicServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaMusicServer/debug/ -lRogalunaMusicServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaMusicServer/ -lRogalunaMusicServer

INCLUDEPATH += $$PWD/../RogalunaMusicServer
DEPENDPATH += $$PWD/../RogalunaMusicServer
