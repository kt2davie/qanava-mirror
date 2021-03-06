TEMPLATE	= app
TARGET		= test-container
FORMS		= qanMainWindow.ui
CONFIG		+= qt warn_on
DEFINES		+= QANAVA  
LANGUAGE	= C++
QT		+= widgets xml core gui
INCLUDEPATH += ../../src $(QTPROPERTYBROWSER)/src

RESOURCES += container.qrc

SOURCES	+=  qanApp.cpp          \
            qanMainWindow.cpp
HEADERS	+=  qanMainWindow.h

CONFIG(debug, debug|release) {
    linux-g++*: LIBS	+= -L../../build/ -lqanavad -L$(QTPROPERTYBROWSER)/lib -lqtpropertybrowserd
    android:    LIBS	+= -L../../build/ -lqanavad -L$(QTPROPERTYBROWSER)/lib -lqtpropertybrowserd
    win32:      PRE_TARGETDEPS += ../../build/qanavad.lib
    win32:      OBJECTS_DIR = ./Debug
    win32:      LIBS	+= ../../build/qanavad.lib $(QTPROPERTYBROWSER)/lib/libqtpropertybrowserd.lib
}

CONFIG(release, debug|release) {
    linux-g++*: LIBS	+= -L../../build/ -lqanava -L$(QTPROPERTYBROWSER)/lib -lqtpropertybrowser
    android:    LIBS	+= -L../../build/ -lqanava -L$(QTPROPERTYBROWSER)/lib -lqtpropertybrowser
    win32:      PRE_TARGETDEPS += ../../build/qanava.lib
    win32:      OBJECTS_DIR = ./Release
    win32:      LIBS	+= ../../build/qanava.lib $(QTPROPERTYBROWSER)/lib/libqtpropertybrowser.lib
}

