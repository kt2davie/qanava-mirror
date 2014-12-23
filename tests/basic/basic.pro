TEMPLATE	= app
TARGET		= test-basic
FORMS		= qanMainWindow.ui
CONFIG		+= qt warn_on debug
DEFINES		+= QANAVA  
LANGUAGE	= C++
QT		+= widgets core gui
INCLUDEPATH += ../../src $(PROPERTYBROWSER)
  
SOURCES	+=  qanApp.cpp          \
            qanMainWindow.cpp
HEADERS	+=  qanMainWindow.h

CONFIG(debug, debug|release) {
    linux-g++*: LIBS	+= -L../../build/ -lqanavad -L$(PROPERTYBROWSER) -lpropertybrowserd
    android:    LIBS	+= -L../../build/ -lqanavad -L$(PROPERTYBROWSER) -lpropertybrowserd
    win32:      PRE_TARGETDEPS += ../../build/qanavad.lib
    win32:      OBJECTS_DIR = ./Debug
    win32:      LIBS	+= ../../build/qanavad.lib $(PROPERTYBROWSER)/libpropertybrowserd.lib
}

CONFIG(release, debug|release) {
    linux-g++*: LIBS	+= -L../../build/ -lqanava -L$(PROPERTYBROWSER) -lpropertybrowser
    android:    LIBS	+= -L../../build/ -lqanava -L$(PROPERTYBROWSER) -lpropertybrowser
    win32:      PRE_TARGETDEPS += ../../build/qanava.lib
    win32:      OBJECTS_DIR = ./Release
    win32:      LIBS	+= ../../build/qanava.lib $(PROPERTYBROWSER)/libpropertybrowser.lib
}
