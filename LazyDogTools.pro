QT       += core gui svg sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lUser32 -lDbgHelp -lversion -lole32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioHelper/AudioHelper.cpp \
    AudioHelper/AudioHelperManager.cpp \
    AudioHelper/AudioManager.cpp \
    AudioHelper/SelectionDialog.cpp \
    AudioHelper/TaskMonitor.cpp \
    HotkeyManager.cpp \
    LogHandler.cpp \
    Settings.cpp \
    SettingsManager.cpp \
    ToolManager.cpp \
    TransHelper/TransHelper.cpp \
    TransHelper/TransHelperManager.cpp \
    main.cpp \
    LazyDogTools.cpp

HEADERS += \
    AudioHelper/AudioCustom.h \
    AudioHelper/AudioHelper.h \
    AudioHelper/AudioHelperManager.h \
    AudioHelper/AudioManager.h \
    AudioHelper/PolicyConfig.h \
    AudioHelper/SelectionDialog.h \
    AudioHelper/TaskMonitor.h \
    CustomWidget.h \
    HotkeyManager.h \
    LazyDogTools.h \
    LogHandler.h \
    PolicyConfig.h \
    Settings.h \
    SettingsManager.h \
    ToolManager.h \
    TransHelper/TransHelper.h \
    TransHelper/TransHelperManager.h

FORMS += \
    lazydogtools.ui

#设置图标
RC_ICONS = images\ico\LD_64.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    images.qrc

QMAKE_PROJECT_DEPTH = 0

DEFINES += QT_MESSAGELOGCONTEXT
