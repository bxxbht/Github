#-------------------------------------------------
#
# Project created by QtCreator 2014-10-13T10:41:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lakshi
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    settings.cpp \
    crc-ccitt.cpp \
    partition.cpp

HEADERS  += dialog.h \
    settings.h \
    crc-ccitt.h \
    partition.h

FORMS    += dialog.ui \
    partition.ui
