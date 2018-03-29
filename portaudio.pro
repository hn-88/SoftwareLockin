#-------------------------------------------------
#
# Project created by QtCreator 2010-11-21T15:58:01
#
#-------------------------------------------------

QT       += core
# added 26 Mar 2018
QT += widgets

QT       -= gui

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += portaudio.cpp

LIBS += -lportaudio
