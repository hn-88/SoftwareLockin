#-------------------------------------------------
#
# Project created by QtCreator 2011-02-17T16:14:32
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

# added 26 Mar 2018
QT += widgets

TARGET = lockins
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += multi.cpp \
    lockingui.cpp \
    lowpassfilter.cpp \
    lockin.cpp \
    coutwriter.cpp \
    alsainput.cpp \
    vumeter.cpp \
    signalmultiplyer.cpp \
    signalmiddle.cpp \
    phasor.cpp \
    phaselockedloop.cpp \
    coutreflecto.cpp \
    lockinmutlichannels.cpp \
    dockoutput.cpp \
    alsaoutputhz.cpp \
    sigfactormultiplyer.cpp \
    saturation.cpp \
    noise.cpp

HEADERS += \
    lockingui.h \
    lowpassfilter.h \
    lockin.h \
    coutwriter.h \
    alsainput.h \
    vumeter.h \
    signalmultiplyer.h \
    signalmiddle.h \
    phasor.h \
    phaselockedloop.h \
    mstream.h \
    coutreflecto.h \
    lockinmutlichannels.h \
    dockoutput.h \
    alsaoutputhz.h \
    sigfactormultiplyer.h \
    saturation.h \
    noise.h

FORMS += \
    lockingui.ui \
    dockoutput.ui

LIBS += -lasound
#LIBS += -lkdecore -lkdeui

RESOURCES += \
    images.qrc
