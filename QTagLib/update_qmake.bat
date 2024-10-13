@echo off
setlocal EnableDelayedExpansion

REM 定义输出的 .pro 文件名
set OUTPUT_FILE=QTagLib.pro

REM 写入模板头部到 .pro 文件
(
echo QT -= gui
echo.
echo TEMPLATE = lib
echo DEFINES += ROGALUNAMUSICSERVER_LIBRARY
echo.
echo CONFIG += c++17
echo.
echo # You can make your code fail to compile if it uses deprecated APIs.
echo # In order to do so, uncomment the following line.
echo #DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
echo.
) > %OUTPUT_FILE%

REM 开始写入 SOURCES 部分
echo SOURCES += \>> %OUTPUT_FILE%

REM 遍历 taglib 目录下的所有 .cpp 文件
for /R taglib %%f in (*.cpp) do (
    set "relpath=%%~f"
    REM 移除前面的路径，保留相对路径
    set "relpath=!relpath:%cd%\=!"
    REM 将反斜杠替换为正斜杠
    set "relpath=!relpath:\=/!"
    echo     !relpath! \>> %OUTPUT_FILE%
)

REM 开始写入 HEADERS 部分
echo.>> %OUTPUT_FILE%
echo HEADERS += \>> %OUTPUT_FILE%

REM 遍历 taglib 目录下的所有 .h 文件
for /R taglib %%f in (*.h) do (
    set "relpath=%%~f"
    set "relpath=!relpath:%cd%\=!"
    set "relpath=!relpath:\=/!"
    echo     !relpath! \>> %OUTPUT_FILE%
)

REM 写入模板的剩余部分
(
echo.
echo # Default rules for deployment.
echo unix {
echo     target.path = /usr/lib
echo }
echo !isEmpty\(target.path\): INSTALLS += target
) >> %OUTPUT_FILE%
