QT       += core gui svg sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lUser32 -lDbgHelp -lversion -lole32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    LazyDogTools.cpp

SOURCES += \
    managers/SingleApplication.cpp \
    managers/HotkeyManager.cpp \
    managers/LogHandler.cpp \
    managers/Settings.cpp \
    managers/ThemeManager.cpp \
    managers/ToolManager.cpp \
    managers/TrayManager.cpp

SOURCES += \
    models/ToolModel.cpp \
    models/ToolWidgetModel.cpp

SOURCES += \
    widgets/AcrylicWidget.cpp \
    widgets/ColorPickerWidget.cpp \
    widgets/CustomGroupBox.cpp \
    widgets/SettingsWidget.cpp \
    widgets/SmoothScrollArea.cpp \
    widgets/TransparentWidget.cpp

SOURCES += \
    components/CustomKeySequenceEdit.cpp \
    components/HintLabel.cpp \
    components/JumpButton.cpp \
    components/LeftMenuButton.cpp \
    components/MacStyleButton.cpp \
    components/MacStyleCheckBox.cpp \
    components/MacStyleComboBox.cpp \
    components/MacStyleSlider.cpp \
    components/MacSwitchButton.cpp

SOURCES += \
    AudioHelper/AudioDatabase.cpp \
    AudioHelper/AudioHelper.cpp \
    AudioHelper/AudioHelperServer.cpp \
    AudioHelper/AudioHelperWidget.cpp \
    AudioHelper/AudioManager.cpp \
    AudioHelper/SelectionDialog.cpp \
    AudioHelper/TaskMonitor.cpp

HEADERS += \
    LazyDogTools.h

HEADERS += \
    utils/Constants.h \
    utils/Custom.h \
    utils/UAC.h

HEADERS += \
    managers/SingleApplication.h \
    managers/HotkeyManager.h \
    managers/LogHandler.h \
    managers/Settings.h \
    managers/ThemeManager.h \
    managers/ToolManager.h \
    managers/TrayManager.h

HEADERS += \
    models/ToolModel.h \
    models/ToolWidgetModel.h

HEADERS += \
    widgets/AcrylicWidget.h \
    widgets/ColorPickerWidget.h \
    widgets/CustomGroupBox.h \
    widgets/SettingsWidget.h \
    widgets/SmoothScrollArea.h \
    widgets/TransparentWidget.h

HEADERS += \
    components/CustomKeySequenceEdit.h \
    components/HintLabel.h \
    components/JumpButton.h \
    components/LeftMenuButton.h \
    components/MacStyleButton.h \
    components/MacStyleCheckBox.h \
    components/MacStyleComboBox.h \
    components/MacStyleSlider.h \
    components/MacSwitchButton.h

HEADERS += \
    AudioHelper/AudioCustom.h \
    AudioHelper/AudioDatabase.h \
    AudioHelper/AudioHelper.h \
    AudioHelper/AudioHelperServer.h \
    AudioHelper/AudioHelperWidget.h \
    AudioHelper/AudioManager.h \
    AudioHelper/PolicyConfig.h \
    AudioHelper/SelectionDialog.h \
    AudioHelper/TaskMonitor.h

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
VERSION = 0.0.4
# 语言
RC_LANG = 0x0004
# 产品名称
QMAKE_TARGET_PRODUCT = LazyDogTools
# 详细描述
QMAKE_TARGET_DESCRIPTION = LazyDogTools
# 版权
QMAKE_TARGET_COPYRIGHT = Copyright(C) 2024
