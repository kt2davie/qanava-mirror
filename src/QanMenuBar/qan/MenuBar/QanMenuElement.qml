
import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1

Rectangle {
    id: menuElement

    anchors.margins: 5
    state: "RELEASED"
    color: "transparent"
    objectName: "QanMenuElement"    // Do not change menu element object name...

    Layout.alignment: Qt.AlignCenter
    Layout.fillWidth : true
    Layout.fillHeight : true

    Layout.preferredWidth: 32
    Layout.preferredHeight: 32

    Layout.minimumWidth: 16
    Layout.minimumHeight: 16

    Layout.maximumWidth: Number.POSITIVE_INFINITY
    Layout.maximumHeight: Number.POSITIVE_INFINITY

    property string label
    property var    source
    property var    parentMenu : null
    property var    childMenu : null
    property bool   checkable: false

    // Emitted when the element is activated and the user click this element
    signal  activated( );

    // Called by an eventually connected Qt Action (could be used to detect a checked state change with checkable actions).
    function setChecked( checked )
    {
        checkable = true
        console.log( "QanMenuElement::setChecked(): " + label + " is " + checked );
        if ( checked && state !== "CHECKED" )
            state = "CHECKED";
        if ( state === "CHECKED" && !checked )
            state = "RELEASED";
    }

    // An eventual Qt action associed to this menu element.
    property QtObject   action;

    property color hilightGradColor : "#4f4ae1"

    states: [
        State {
            name: "CHECKED"
            PropertyChanges { target: hiligther; opacity : 0.9; }
            PropertyChanges { target: menuElement; hilightGradColor : "#f15050" ; scale : 1.1 }
        },
        State {
            name: "ACTIVATED"
            PropertyChanges { target: hiligther; opacity : 0.8 }
            PropertyChanges { target: menuElement; scale : 1.1 }
            StateChangeScript {
                script: {
                    if ( parentMenu )
                    {
                        parentMenu.releaseAllMenuElement( menuElement );
                        parentMenu.state = "ACTIVATED"; // Force parent menu activation (avoid release once an item is activated)
                    }
                    if ( childMenu )
                        childMenu.state = "ACTIVATED";
                    if ( parentMenu && parentMenu.parentElement )
                        parentMenu.parentElement.state = "ACTIVATED";
                }
            }
        },
        State {
            name: "RELEASED"
            PropertyChanges { target: hiligther; opacity : 0. }
            PropertyChanges { target: menuElement; scale : 1. }
            StateChangeScript {
                script: {
                    if ( childMenu )
                        childMenu.state = "RELEASED";
                }
            }
        }
    ]

    transitions: [
        Transition {
            from: "ACTIVATED"
            to: "RELEASED"
            NumberAnimation { target: hiligther; properties: "opacity"; duration: 400 }
            NumberAnimation { target: menuElement; properties: "scale"; easing.type: Easing.OutElastic; duration: 400 }
        },
        Transition {
            from: "RELEASED"
            to: "ACTIVATED"
            NumberAnimation { target: hiligther; properties: "opacity"; duration: 200 }
            NumberAnimation { target: menuElement; properties: "scale"; easing.type: Easing.OutElastic; duration: 200 }
        }
    ]

    ColumnLayout {
        id: elementLayout
        anchors.fill: parent
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth : true
        Layout.fillHeight : true

        Layout.preferredWidth: 64
        Layout.preferredHeight: 64

        Layout.minimumWidth: 32
        Layout.minimumHeight: 32

        Layout.maximumWidth: Number.POSITIVE_INFINITY
        Layout.maximumHeight: Number.POSITIVE_INFINITY

        spacing: 1
        Image {
            id: menuElementImage
            source: menuElement.source
            fillMode: Image.PreserveAspectFit
            antialiasing: true

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth : true
            Layout.fillHeight : true

            Layout.preferredWidth: 32
            Layout.preferredHeight: 32

            Layout.minimumWidth: 16
            Layout.minimumHeight: 16

            Layout.maximumWidth: Number.POSITIVE_INFINITY
            Layout.maximumHeight: Number.POSITIVE_INFINITY
        }
        Text {
            id: menuElementLabel
            text : menuElement.label
            Layout.alignment: Qt.AlignCenter
            color: "white"
        }
    }

    Image { // check image to differentiate checkable menu elements
        id: checkableIcon
        visible: menuElement.checkable
        opacity: 0.9

        source: "checkbox.svg"

        x: parent.width - 15
        y: menuElementImage.height - 6
        width: 12
        height: 12
    }
    Image { // check image to differentiate checkable menu elements
        id: subMenuIcon
        visible: menuElement.childMenu != null
        opacity: 0.9

        source: "arrow-right.svg"

        x: parent.width - 15
        y: menuElementImage.height - 6
        width: 12
        height: 12
    }

    Rectangle {
        id: hiligther
        anchors.fill: parent

        // Hilight rectangle fill gradient
        gradient: Gradient {
            GradientStop {
                position: 0.00;
                color: "#00a3b5ff";
            }
            GradientStop {
                position: 0.50;
                color: hilightGradColor;
            }
            GradientStop {
                position: 1.00;
                color: "#00ccc7ff";
            }
        }

        border.width: 0
        enabled: true
        antialiasing: true
        opacity: 0.0

        MouseArea {
            id: elementMouseArea
            opacity: 1
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                if ( menuElement.state !== "CHECKED" )
                    menuElement.state = "ACTIVATED";
            }
            onExited: {
                // Do not force the RELEASED mode since child and parent menus are "leaders" for releasing menu elements
            }
            onClicked: {
                console.log( "QanMenuElement::onClicked(): " + menuElement.label );
                menuElement.activated( );
            }
        }
    }
}
