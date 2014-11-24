
TEMPLATE	= lib
LANGUAGE	= C++
DEFINES		+= QANAVA
TARGET		= qanava
DESTDIR		= ../build
CONFIG		+= debug warn_on qt thread staticlib
INCLUDEPATH	+= $(PROPERTYBROWSER)
QT		+= widgets xml core
 
HEADERS +=	./qanConfig.h \
                ./qanEdge.h \
                ./qanGraph.h \
                ./qanGrid.h \
                ./qanLayout.h \
                ./qanSimpleLayout.h \
                ./qanTreeLayout.h \
                ./qanNode.h \
                ./qanController.h \
                ./qanSimpleNodeItem.h \
                ./qanNodeItem.h \
                ./qanEdgeItem.h \
                ./qanGraphItem.h \
                ./qanProperties.h \
                ./qanStyleManager.h \
                ./qanGraphView.h \
                ./qanGraphModel.h \
                ./qanGraphScene.h \
                ./qanNodeGroup.h \
                ./qanGridItem.h \
                ./ui/uiStyleEditorWidget.h \
                ./ui/uiStyleBrowserWidget.h \
                ./ui/uiNodeGroupFilterWidget.h \
                ./ui/uiNodesItemModel.h \
                ./ui/uiUtil.h		\
                ./qanContainerGraphicsItem.h
	
SOURCES += 	./qanEdge.cpp                       \
                ./qanGraph.cpp                      \
                ./qanLayout.cpp                     \
                ./qanSimpleLayout.cpp               \
                ./qanTreeLayout.cpp                 \
                ./qanNode.cpp                       \
                ./qanController.cpp                 \
                ./qanSimpleNodeItem.cpp             \
                ./qanNodeItem.cpp                   \
                ./qanEdgeItem.cpp                   \
                ./qanProperties.cpp                 \
                ./qanStyleManager.cpp               \
                ./qanGraphView.cpp                  \
                ./qanGraphModel.cpp                 \
                ./qanGraphScene.cpp                 \
                ./qanNodeGroup.cpp                  \
                ./qanGraphItem.cpp                  \
                ./qanGridItem.cpp                   \
                ./ui/uiStyleEditorWidget.cpp        \
                ./ui/uiStyleBrowserWidget.cpp       \
                ./ui/uiNodeGroupFilterWidget.cpp    \
                ./ui/uiNodesItemModel.cpp           \
                ./qanContainerGraphicsItem.cpp

android {
    Debug:TARGET    = qanavad
    Release:TARGET  = qanava
}

win32 {
    Debug:TARGET	= qanavad
    Debug:LIBS		+= $(PROPERTYBROWSER)/qtpropertybrowserd.lib
    Release:TARGET	= qanava
    Release:LIBS	+= $(PROPERTYBROWSER)/qtpropertybrowser.lib
}

