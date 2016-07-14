#-------------------------------------------------
#
# Project created by QtCreator 2016-04-13T11:48:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = console
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../partition.cpp \
    ../hal.cpp

HEADERS += \
    ../partition.h \
    ../hal.h
