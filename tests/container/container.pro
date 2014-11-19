TEMPLATE	= app
TARGET		= test-container
FORMS		= qanMainWindow.ui
CONFIG		+= qt warn_on debug
DEFINES		+= QANAVA  
LANGUAGE	= C++
QT			+= widgets xml core gui

SOURCES	+= qanApp.cpp \
		   qanMainWindow.cpp
HEADERS	+= qanMainWindow.h 

win32 {
  INCLUDEPATH += ../../src $(PROPERTYBROWSER)
  OBJECTS_DIR = ./Debug
  Debug:LIBS	+= ../../build/qanavad.lib $(PROPERTYBROWSER)/qtpropertybrowserd.lib
  Release:LIBS	+= ../../build/qanava.lib $(PROPERTYBROWSER)/qtpropertybrowser.lib
}


