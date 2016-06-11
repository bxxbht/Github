TEMPLATE = app
TARGET = tpcap
DEPENDPATH += .
INCLUDEPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

QT = core gui network

RESOURCES = client.qrc

include(src/qtsingleapplication-2.6_1-opensource/src/qtsingleapplication.pri)

# Input
HEADERS += src/configuration.h src/mainwindow.h

SOURCES += src/configuration.cpp src/main.cpp src/mainwindow.cpp
