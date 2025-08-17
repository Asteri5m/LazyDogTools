QT       += core gui svg sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lUser32 -lDbgHelp -lversion -lole32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioHelper/AudioDatabase.cpp \
    AudioHelper/AudioHelper.cpp \
    AudioHelper/AudioHelperServer.cpp \
    AudioHelper/AudioHelperWidget.cpp \
    AudioHelper/AudioManager.cpp \
    AudioHelper/SelectionDialog.cpp \
    AudioHelper/TaskMonitor.cpp \
    HotkeyManager.cpp \
    LazyDogTools.cpp \
    LogHandler.cpp \
    Settings.cpp \
    SettingsWidget.cpp \
    SingleApplication.cpp \
    ToolManager.cpp \
    ToolModel.cpp \
    TrayManager.cpp \
    main.cpp

HEADERS += \
    AudioHelper/AudioCustom.h \
    AudioHelper/AudioDatabase.h \
    AudioHelper/AudioHelper.h \
    AudioHelper/AudioHelperServer.h \
    AudioHelper/AudioHelperWidget.h \
    AudioHelper/AudioManager.h \
    AudioHelper/PolicyConfig.h \
    AudioHelper/SelectionDialog.h \
    AudioHelper/TaskMonitor.h \
    Custom.h \
    CustomWidget.h \
    HotkeyManager.h \
    LazyDogTools.h \
    LogHandler.h \
    Settings.h \
    SettingsWidget.h \
    SingleApplication.h \
    ToolManager.h \
    ToolModel.h \
    TrayManager.h \
    UAC.h

#设置图标
RC_ICONS = images\ico\favicon_32.ico \
           images\ico\favicon_64.ico \
           images\ico\favicon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    images.qrc \
    text.qrc

QMAKE_PROJECT_DEPTH = 0

DEFINES += QT_MESSAGELOGCONTEXT

# 版本信息
VERSION = 0.0.1

# 语言
RC_LANG = 0x0004
# 产品名称
QMAKE_TARGET_PRODUCT = LazyDogTools
# 详细描述
QMAKE_TARGET_DESCRIPTION = LazyDogTools
# 版权
QMAKE_TARGET_COPYRIGHT = Copyright(C) 2024
