TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

#CONFIG = qt staticlib static
CONFIG = qt

#LIBS =  ../lib/libQtNetwork.a ../lib/libQtCore.a ../lib/libQtGui.a

OBJECTS_DIR = obj
MOC_DIR = moc

QT = core gui network

# Input
HEADERS += combobox.h communication.h configuration.h mainwindow.h \
            qextserialport_global.h qextserialport.h spinbox.h

SOURCES += combobox.cpp communication.cpp configuration.cpp main.cpp mainwindow.cpp \
            posix_qextserialport.cpp qextserialport.cpp spinbox.cpp

