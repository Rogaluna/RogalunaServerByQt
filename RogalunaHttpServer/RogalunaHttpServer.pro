QT -= gui
QT += network
QT += httpserver

TEMPLATE = lib
DEFINES += ROGALUNAHTTPSERVER_LIBRARY

CONFIG += c++17
CONFIG += object_parallel_to_source # 编译配置，设置了这个选项，o文件将依照项目目录生成

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RequestHandlers/Account/GetAccountInfoHandler.cpp \
    RequestHandlers/Account/LoginHandler.cpp \
    RequestHandlers/Account/ModifyAccountInfoHandler.cpp \
    RequestHandlers/Account/RegisterHandler.cpp \
    RequestHandlers/CloudDrive/DeleteFileHandler.cpp \
    RequestHandlers/CloudDrive/GetFileDetailHandler.cpp \
    RequestHandlers/CloudDrive/GetUidFromPathHandler.cpp \
    RequestHandlers/CloudDrive/RenameFileHandler.cpp \
    RequestHandlers/DefaultOptionsHandler.cpp \
    RequestHandlers/CloudDrive/CreateFolderHandler.cpp \
    RequestHandlers/CloudDrive/FetchFileDirectLinkHandler.cpp \
    RequestHandlers/CloudDrive/GetFileHandler.cpp \
    RequestHandlers/CloudDrive/GetFileListHandler.cpp \
    RequestHandlers/CloudDrive/GetParentFolderHandler.cpp \
    RequestHandlers/CloudDrive/MergeFileHandler.cpp \
    RequestHandlers/CloudDrive/PostFileHandler.cpp \
    RequestHandlers/GetWebFileHandler.cpp \
    RequestHandlers/Library/DeleteBookHandler.cpp \
    RequestHandlers/Library/DeleteChapterHandler.cpp \
    RequestHandlers/Library/GetBookCategoriesHandler.cpp \
    RequestHandlers/Library/GetBookCoverHandler.cpp \
    RequestHandlers/Library/GetBookInfoHandler.cpp \
    RequestHandlers/Library/GetBookListHandler.cpp \
    RequestHandlers/Library/GetBookReadProgressHandler.cpp \
    RequestHandlers/Library/GetCategoriesHandler.cpp \
    RequestHandlers/Library/GetChapterContentHandler.cpp \
    RequestHandlers/Library/GetChapterInfoHandler.cpp \
    RequestHandlers/Library/GetChapterListHandler.cpp \
    RequestHandlers/Library/GetChapterResourceHandler.cpp \
    RequestHandlers/Library/NewBookHandler.cpp \
    RequestHandlers/Library/NewChapterHandler.cpp \
    RequestHandlers/Library/UpdateBookInfoHandler.cpp \
    RequestHandlers/Library/UpdateBookReadProgressHandler.cpp \
    RequestHandlers/Library/UpdateChapterContentHandler.cpp \
    RequestHandlers/Library/UpdateChapterInfoHandler.cpp \
    RequestHandlers/Library/UploadBookCoverHandler.cpp \
    RequestHandlers/Library/UploadChapterResourceHandler.cpp \
    RequestHandlers/MusicStation/GetAlbumsInfoHandler.cpp \
    RequestHandlers/MusicStation/GetMusicCoverHandler.cpp \
    RequestHandlers/MusicStation/GetMusicHandler.cpp \
    RequestHandlers/MusicStation/GetMusicListHandler.cpp \
    RequestHandlers/MusicStation/MergeMusicHandler.cpp \
    RequestHandlers/MusicStation/PostAlbumImageHandler.cpp \
    RequestHandlers/MusicStation/PostMusicHandler.cpp \
    RogalunaHttpServer.cpp

HEADERS += \
    Macro/MD5HashGenerator.h \
    Macro/RequestBodyParser.h \
    Macro/TokenGV.h \
    RequestHandlers/Account/GetAccountInfoHandler.h \
    RequestHandlers/Account/LoginHandler.h \
    RequestHandlers/Account/ModifyAccountInfoHandler.h \
    RequestHandlers/Account/RegisterHandler.h \
    RequestHandlers/CloudDrive/DeleteFileHandler.h \
    RequestHandlers/CloudDrive/GetFileDetailHandler.h \
    RequestHandlers/CloudDrive/GetUidFromPathHandler.h \
    RequestHandlers/CloudDrive/RenameFileHandler.h \
    RequestHandlers/DefaultOptionsHandler.h \
    RequestHandlers/CloudDrive/CreateFolderHandler.h \
    RequestHandlers/CloudDrive/FetchFileDirectLinkHandler.h \
    RequestHandlers/CloudDrive/GetFileHandler.h \
    RequestHandlers/CloudDrive/GetFileListHandler.h \
    RequestHandlers/CloudDrive/GetParentFolderHandler.h \
    RequestHandlers/CloudDrive/MergeFileHandler.h \
    RequestHandlers/CloudDrive/PostFileHandler.h \
    RequestHandlers/GetWebFileHandler.h \
    RequestHandlers/Library/DeleteBookHandler.h \
    RequestHandlers/Library/DeleteChapterHandler.h \
    RequestHandlers/Library/GetBookCategoriesHandler.h \
    RequestHandlers/Library/GetBookCoverHandler.h \
    RequestHandlers/Library/GetBookInfoHandler.h \
    RequestHandlers/Library/GetBookListHandler.h \
    RequestHandlers/Library/GetBookReadProgressHandler.h \
    RequestHandlers/Library/GetCategoriesHandler.h \
    RequestHandlers/Library/GetChapterContentHandler.h \
    RequestHandlers/Library/GetChapterInfoHandler.h \
    RequestHandlers/Library/GetChapterListHandler.h \
    RequestHandlers/Library/GetChapterResourceHandler.h \
    RequestHandlers/Library/NewBookHandler.h \
    RequestHandlers/Library/NewChapterHandler.h \
    RequestHandlers/Library/UpdateBookInfoHandler.h \
    RequestHandlers/Library/UpdateBookReadProgressHandler.h \
    RequestHandlers/Library/UpdateChapterContentHandler.h \
    RequestHandlers/Library/UpdateChapterInfoHandler.h \
    RequestHandlers/Library/UploadBookCoverHandler.h \
    RequestHandlers/Library/UploadChapterResourceHandler.h \
    RequestHandlers/MusicStation/GetAlbumsInfoHandler.h \
    RequestHandlers/MusicStation/GetMusicCoverHandler.h \
    RequestHandlers/MusicStation/GetMusicHandler.h \
    RequestHandlers/MusicStation/GetMusicListHandler.h \
    RequestHandlers/MusicStation/MergeMusicHandler.h \
    RequestHandlers/MusicStation/PostAlbumImageHandler.h \
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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RogalunaAccountServer/release/ -lRogalunaAccountServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RogalunaAccountServer/debug/ -lRogalunaAccountServer
else:unix:!macx: LIBS += -L$$OUT_PWD/../RogalunaAccountServer/ -lRogalunaAccountServer

INCLUDEPATH += $$PWD/../RogalunaAccountServer
DEPENDPATH += $$PWD/../RogalunaAccountServer
