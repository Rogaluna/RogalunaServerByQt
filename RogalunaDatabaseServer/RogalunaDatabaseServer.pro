QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += ROGALUNADATABASESERVER_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BaseDAO.cpp \
    Database/Account/UsersDAO.cpp \
    Database/CloudDrive/ContentDAO.cpp \
    Database/CloudDrive/MetadataDAO.cpp \
    Database/Library/CategoriesDAO.cpp \
    Database/TestDAO.cpp \
    RogalunaDatabaseServer.cpp

HEADERS += \
    BaseDAO.h \
    Database/Account/UsersDAO.h \
    Database/CloudDrive/ContentDAO.h \
    Database/CloudDrive/MetadataDAO.h \
    Database/Library/CategoriesDAO.h \
    Database/TestDAO.h \
    Interface/Account/IUsersDAO.h \
    Interface/CloudDrive/IContentDAO.h \
    Interface/CloudDrive/IMetadataDAO.h \
    Interface/Library/ICategoriesDAO.h \
    RogalunaDatabaseServer.h \
    RogalunaDatabaseServer_Global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
