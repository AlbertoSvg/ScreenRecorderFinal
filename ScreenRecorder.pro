TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Recorder.cpp \
        main.cpp

HEADERS += \
    Recorder.h

unix:LIBS += -lavformat -lavcodec -lavutil -lavdevice -lm -lswscale -lX11 -lpthread -lswresample -lasound

INCLUDEPATH += $$PWD/LINUX/include
DEPENDPATH += $$PWD/LINUX/include
