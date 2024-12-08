QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += ROGALUNADATABASESERVER_LIBRARY

CONFIG += c++17
CONFIG += object_parallel_to_source # 编译配置，设置了这个选项，o文件将依照项目目录生成

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BaseDAO.cpp \
    Database/Library/BookCategoriesDAO.cpp \
    Database/Library/BooksDAO.cpp \
    Database/Library/ChaptersDAO.cpp \
    Database/Library/ReadProgressDAO.cpp \
    Database/Library/UserCollectionsDAO.cpp \
    Database/MusicStation/AlbumsDAO.cpp \
    Database/MusicStation/ContentDAO.cpp \
    HeartbeatTimer.cpp \
    RogalunaDatabaseServer.cpp \
    Database/TestDAO.cpp \
    Database/Account/UsersDAO.cpp \
    Database/CloudDrive/ContentDAO.cpp \
    Database/CloudDrive/MetadataDAO.cpp \
    Database/Library/CategoriesDAO.cpp \
    Database/MusicStation/MetadataDAO.cpp

HEADERS += \
    BaseDAO.h \
    Database/Account/UsersDAO.h \
    Database/CloudDrive/ContentDAO.h \
    Database/CloudDrive/MetadataDAO.h \
    Database/Library/BookCategoriesDAO.h \
    Database/Library/BooksDAO.h \
    Database/Library/CategoriesDAO.h \
    Database/Library/ChaptersDAO.h \
    Database/Library/ReadProgressDAO.h \
    Database/Library/UserCollectionsDAO.h \
    Database/MusicStation/AlbumsDAO.h \
    Database/MusicStation/ContentDAO.h \
    Database/MusicStation/MetadataDAO.h \
    Database/TestDAO.h \
    HeartbeatTimer.h \
    Interface/Account/IUsersDAO.h \
    Interface/CloudDrive/IContentDAO.h \
    Interface/CloudDrive/IMetadataDAO.h \
    Interface/Library/IBookCategoriesDAO.h \
    Interface/Library/IBooksDAO.h \
    Interface/Library/ICategoriesDAO.h \
    Interface/Library/IChaptersDAO.h \
    Interface/Library/IReadProgressDAO.h \
    Interface/Library/IUserCollectionsDAO.h \
    Interface/MusicStation/IAlbumsDAO.h \
    Interface/MusicStation/IContentDAO.h \
    Interface/MusicStation/IMetaDataDAO.h \
    RogalunaDatabaseServer.h \
    RogalunaDatabaseServer_Global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
