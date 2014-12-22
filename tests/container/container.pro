TEMPLATE	= app
TARGET		= test-container
FORMS		= qanMainWindow.ui
CONFIG		+= qt warn_on debug
DEFINES		+= QANAVA  
LANGUAGE	= C++
QT		+= widgets xml core gui
INCLUDEPATH += ../../src $(PROPERTYBROWSER)

RESOURCES += container.qrc

SOURCES	+=  qanApp.cpp          \
            qanMainWindow.cpp
HEADERS	+=  qanMainWindow.h

android {
    LIBS	+= -L../../build/ -lqanava -L$(PROPERTYBROWSER) -lpropertybrowserd
    Debug:LIBS	+= -L../../build/ -lqanavad -L$(PROPERTYBROWSER) -lpropertybrowserd
    Release:LIBS	+= -L../../build/ -lqanava -L$(PROPERTYBROWSER) -lpropertybrowser
}

win32 {
    Debug:PRE_TARGETDEPS += ../../build/qanavad.lib
    Release:PRE_TARGETDEPS += ../../build/qanava.lib

    Debug:OBJECTS_DIR = ./Debug
    Release:OBJECTS_DIR = ./Release
    Debug:LIBS	+= ../../build/qanavad.lib $(PROPERTYBROWSER)/libpropertybrowserd.lib
    Release:LIBS	+= ../../build/qanava.lib $(PROPERTYBROWSER)/libpropertybrowser.lib
}


