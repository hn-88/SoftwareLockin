#-------------------------------------------------
#
# Project created by QtCreator 2010-12-22T16:22:50
#
#-------------------------------------------------

QT       += core gui
QT       += opengl
# added 26 Mar 2018
QT += widgets

TARGET = lockin
TEMPLATE = app


SOURCES += single.cpp\
    vumeter.cpp \
    alsainput.cpp \
    lowpassfilter.cpp \
    signalmultiplyer.cpp \
    phaselockedloop.cpp \
    coutwriter.cpp \
    lockingui.cpp \
    lockin.cpp \
    signalmiddle.cpp \
    phasor.cpp \
    sigfactormultiplyer.cpp \
    alsaoutputhz.cpp \
    saturation.cpp \
    noise.cpp

HEADERS  += \
    vumeter.h \
    mstream.h \
    alsainput.h \
    lowpassfilter.h \
    signalmultiplyer.h \
    phaselockedloop.h \
    coutwriter.h \
    lockingui.h \
    lockin.h \
    signalmiddle.h \
    phasor.h \
    sigfactormultiplyer.h \
    alsaoutputhz.h \
    saturation.h \
    noise.h

LIBS += -lasound

FORMS += \
    lockingui.ui

RESOURCES += \
    images.qrc
