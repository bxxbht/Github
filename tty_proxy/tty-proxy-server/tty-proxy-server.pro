QT = core gui network

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tty-proxy-server
TEMPLATE = app

# Input
HEADERS += combobox.h communication.h configuration.h mainwindow.h \
            qextserialport_global.h qextserialport.h spinbox.h

SOURCES += combobox.cpp communication.cpp configuration.cpp main.cpp mainwindow.cpp \
            posix_qextserialport.cpp qextserialport.cpp spinbox.cpp

RESOURCES += \
    tty-proxy-server.qrc

OTHER_FILES += \
    default.qss

