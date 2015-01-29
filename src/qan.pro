
TEMPLATE	= lib
LANGUAGE	= C++
DEFINES		+= QANAVA
TARGET		= qanava
DESTDIR		= ../build
CONFIG		+= warn_on qt thread staticlib
INCLUDEPATH	+= $(QTPROPERTYBROWSER)/src
QT		+= core widgets gui xml
 
HEADERS +=	./qanConfig.h                   \
                ./qanEdge.h                     \
                ./qanGraph.h                    \
                ./qanGrid.h                     \
                ./qanLayout.h                   \
                ./qanSimpleLayout.h             \
                ./qanTreeLayout.h               \
                ./qanNode.h                     \
                ./qanController.h               \
                ./qanNodeItem.h                 \
                ./qanNodeRectItem.h             \
                ./qanEdgeItem.h                 \
                ./qanGraphItem.h                \
                ./qanProperties.h               \
                ./qanStyleManager.h             \
                ./qanGraphView.h                \
                ./qanGraphModel.h               \
                ./qanGraphScene.h               \
                ./qanNodeGroup.h                \
                ./qanGridItem.h                 \
                ./ui/uiStyleEditorWidget.h      \
                ./ui/uiStyleBrowserWidget.h     \
                ./ui/uiNodeGroupFilterWidget.h  \
                ./ui/uiNodesItemModel.h         \
                ./ui/uiUtil.h                   \
                ./qanGraphicsController.h       \
                ./qanGraphicsResizer.h          \
                ./qanGraphicsContainer.h
	
SOURCES += 	./qanEdge.cpp                       \
                ./qanGraph.cpp                      \
                ./qanLayout.cpp                     \
                ./qanSimpleLayout.cpp               \
                ./qanTreeLayout.cpp                 \
                ./qanNode.cpp                       \
                ./qanController.cpp                 \
                ./qanNodeItem.cpp                   \
                ./qanNodeRectItem.cpp               \
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
                ./qanGraphicsController.cpp         \
                ./qanGraphicsResizer.cpp            \
                ./qanGraphicsContainer.cpp

CONFIG(release, debug|release) {
    linux-g++*: TARGET    = qanava
    android:    TARGET    = qanava
    win32:      TARGET    = qanava
}

CONFIG(debug, debug|release) {
    linux-g++*: TARGET    = qanavad
    android:    TARGET    = qanavad
    win32:      TARGET    = qanavad
}


