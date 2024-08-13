QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioHelper/AudioHelper.cpp \
    AudioHelper/AudioHelperManager.cpp \
    Settings.cpp \
    SettingsWidget.cpp \
    ToolManager.cpp \
    TransHelper/TransHelper.cpp \
    TransHelper/TransHelperManager.cpp \
    main.cpp \
    LazyDogTools.cpp

HEADERS += \
    AudioHelper/AudioHelper.h \
    AudioHelper/AudioHelperManager.h \
    CustomWidget.h \
    LazyDogTools.h \
    Settings.h \
    SettingsWidget.h \
    ToolManager.h \
    TransHelper/TransHelper.h \
    TransHelper/TransHelperManager.h

FORMS += \
    lazydogtools.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    images.qrc
