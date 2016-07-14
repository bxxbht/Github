#-------------------------------------------------
#
# Project created by QtCreator 2014-09-25T14:52:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = maya
TEMPLATE = app

UI_DIR = ui
RCC_DIR = rcc
MOC_DIR = moc
OBJECTS_DIR = objs

QMAKE_CXXFLAGS += -fexceptions

SOURCES += main.cpp\
        dialog.cpp \
        partition.cpp \
        hal.cpp \
    combobox.cpp

HEADERS += dialog.h \
        partition.h \
        hal.h \
    combobox.h

FORMS   += dialog.ui

OTHER_FILES += \
    default.qss

RESOURCES += \
    maya.qrc
