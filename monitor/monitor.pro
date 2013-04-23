TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

CONFIG = qt

OBJECTS_DIR = .obj
MOC_DIR = .moc

# Input
HEADERS += alarmbar.h \
	   alarmswitch.h \
	   alertbox.h \
	   cardiotachometer.h \
	   channelpanel.h \
	   curveplotter.h \
	   digitalclock.h \
	   ecgchannel.h \
	   iconwidget.h \
	   label.h \
	   mainwindow.h \
	   netstatus.h \
	   nbptrend.h \
	   parameterpanel.h \
	   preferencedialog.h \
	   powerstatus.h \
	   shortcutmenu.h \
	   statusbar.h \
	   suresignstyle.h \
	   tabdialog.h \
	   thermometer.h \
	   userinfo.h \
	   xfifo.h

SOURCES += alarmbar.cpp \
	   alarmswitch.cpp \
	   alertbox.cpp \
	   cardiotachometer.cpp \
	   channelpanel.cpp \
	   curveplotter.cpp \
	   digitalclock.cpp \
	   ecgchannel.cpp \
	   iconwidget.h \
	   iconwidget.cpp \
	   label.cpp \
	   main.cpp \
	   mainwindow.cpp \
	   netstatus.cpp \
	   nbptrend.cpp \
	   parameterpanel.cpp \
	   powerstatus.cpp \
	   preferencedialog.cpp \
	   shortcutmenu.cpp \
	   statusbar.cpp \
	   suresignstyle.cpp \
	   tabdialog.cpp \
	   thermometer.cpp \
	   userinfo.cpp
