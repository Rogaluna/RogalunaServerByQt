QT -= gui
QT += core
QT += widgets


TEMPLATE = lib
DEFINES += ROGALUNASTORAGESERVER_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Windows 特定的配置
win32 {
    LIBS += -lws2_32
    LIBS += -lmswsock
}

# Linux 特定的配置
unix {
    # Linux 下不需要额外的库链接
    # 如果需要，可以在这里添加其他库链接
}


SOURCES += \
    rogalunastorageserver.cpp

HEADERS += \
    RogalunaStorageServer_global.h \
    rogalunastorageserver.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
