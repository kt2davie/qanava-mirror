
TEMPLATE    = app
TARGET      = qmlmenubar
CONFIG      += qt warn_on debug
LANGUAGE    = C++
QT          += core widgets gui qml quick quickwidgets
INCLUDEPATH +=
RESOURCES   += qmlmenubar.qrc

SOURCES     +=  app.cpp             			\
                window.cpp          			\
                ./QanMenuBar/qanQmlMenuBar.cpp

HEADERS     +=  window.h            			\
                ./QanMenuBar/qanQmlMenuBar.h

DISTFILES +=

