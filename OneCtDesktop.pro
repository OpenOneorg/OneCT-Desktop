QT += widgets network

CONFIG += c++17 console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = OneCtDesktop

SOURCES += src/main.cpp \
           src/ApiClient.cpp \
           src/LoginWindow.cpp \
           src/MainWindow.cpp \
           src/PostWidget.cpp \
           src/SettingsDialog.cpp

HEADERS += \
    src/MainWindow.h \
    src/ApiClient.h \
    src/SettingsDialog.h \
    src/PostWidget.h \
    src/LoginWindow.h
    
RESOURCES += resources.qrc

DEFINES += QT_DEPRECATED_WARNINGS

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
