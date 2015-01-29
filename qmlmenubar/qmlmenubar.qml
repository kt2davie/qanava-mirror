
import QtQuick 2.0
import "qrc:/QanMenuBar/qan/MenuBar"

QanMenuBar {

    QanMenuStyle {
        id: myStyle

        property bool debug: false;
        property bool expandedAtStartup : false;
        property real menuOpacity : 1.0
        property color textColor : "#232323"
        property bool showShadows : true

        property QtObject element : QtObject {
            id : elementStyle
            property int height: 70;

            property color hilightGradColor : "#4f4ae1"
            property color checkedGradColor : "#f971ee"
            property color hilightCheckedGradColor : "#ff50f1"
        }

        property color separatorColor : "black"

        property QtObject menu : QtObject {
            property QtObject border : QtObject {
                property int width: 0;
                property color color: "transparent";
                property int radius: 0;
            }

            property Gradient gradient : Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }
    }

    id: myMenuBar
    style : myStyle
    //style.expandedAtStartup : true
    //style.debug: true
    anchors.fill: parent    // Mandatory

    // FIXME: uncomment for a pure declarative menu definition example
   /* QanMenuElement {
        id: fileMenuElement
        source: "qrc:/oxygen/document-open-folder.svg"
        parentMenu: myMenuBar
        childMenu: fileMenu
        label: "File"

        QanMenu {
            id: fileMenu
            label: "File"
            parentElement: fileMenuElement

            QanMenuElement {
                id: fileMenuOpenElement
                source: "qrc:/oxygen/document-open.svg"
                parentMenu: fileMenu
                label: "Open"
            }
            QanMenuElement {
                id: fileMenuSaveAsElement
                source: "qrc:/oxygen/document-save-as.svg"
                parentMenu: fileMenu
                label: "Save As"
            }
            QanMenuElement {
                id: fileMenuExitElement
                source: "qrc:/oxygen/application-exit.svg"
                parentMenu: fileMenu
                label: "Exit"
            }

            Component.onCompleted: {
                // Reparent menu element to this menu layout
                fileMenuOpenElement.parent = fileMenu.layout
                fileMenuSaveAsElement.parent = fileMenu.layout
                fileMenuExitElement.parent = fileMenu.layout
            }
        }
    }

    QanMenuSeparator { id: separator }

    QanMenuElement{
        id: editStylesMenuElement
        source: "qrc:/oxygen/media-playlist.svg"
        label: "Edit Styles"
        parentMenu: myMenuBar
    }
    QanMenuElement{
        id: showStyleBrowserMenuElement
        source: "qrc:/oxygen/office-chart-ring.svg"
        label: "Style \nBrowser"
        parentMenu: myMenuBar
    }

    Component.onCompleted: {
        // Reparent menu element to this menu layout
        fileMenuElement.parent = myMenuBar.mainMenu
        separator.parent = myMenuBar.layout
        editStylesMenuElement.parent = myMenuBar.layout
        showStyleBrowserMenuElement.parent = myMenuBar.layout
        myMenuBar.elementCount += 2;
    }*/
}

