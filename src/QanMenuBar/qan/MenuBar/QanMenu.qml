
import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1


Rectangle {
    id: menu

    // Menu style sheet
    property QanMenuStyle style : QanMenuStyle { }

    x: parent.x + parent.width
    y: parent.height / 5
    width: 70
    height: ( elementCount * 70 + separatorCount * 10 > 80 ? elementCount * 60 + separatorCount * 10 : 80);
    //height: 180

    radius: style.border.radius
    border.width: style.border.width
    border.color: style.border.color

    visible: false
    gradient: style.gradient

    state: "RELEASED"

    property int    elementCount: 0
    property int    separatorCount: 0

    // Menu desired user height
    property real   menuHeight: ( elementCount * 70 + separatorCount * 10 > 80 ? elementCount * 60 + separatorCount * 10 : 80)
    //property real   menuHeight: height

    // Set to true if the menu is not a root menu
    property bool   root: false

    property ColumnLayout layout: menuLayout

    // Menu label
    property string label: ""

    // Menu parent element (can be null)
    property QanMenuElement parentElement

    // Menu mouse area used to release the menu when the mouse exits
    MouseArea {
        id: menuMouseArea
        opacity: 1
        anchors.fill: parent
        z: menu.z + 1.
        hoverEnabled: true
        propagateComposedEvents: true
        onEntered: {

        }

        onExited: {
            console.log( "QanMenu::MouseArea::onExited(): " + menu + " / " + menu.label );
            if ( root ) {   // For root menu, release all menu elements
                for ( var c = 0; c < menuLayout.children.length; c++ )
                    if ( menuLayout.children[ c ].state !== "CHECKED" )
                        menuLayout.children[ c ].state = "RELEASED" ;
            }
        }
    }

    Image {
        id: menuHandler
        //source: "arrow-right.svg"
        source: "arrow-right.svg"
        fillMode: Image.Stretch
        antialiasing: true
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 2
        visible: false
        z: menu.z + 1.

        MouseArea {
            id: menuHandlerMouseArea
            opacity: 1
            anchors.fill: parent
            z: menuHandler.z + 1.
            hoverEnabled: true
            propagateComposedEvents: true
            onEntered: {
                console.log( "QanMenu:menuHandlerMouseArea::onEntered()" );
                menu.state = "ACTIVATED"
            }
        }
    }

    ColumnLayout {
        id: menuLayout
        anchors.fill: parent
        anchors.margins: 7  // Must not be zero or mouse area onExited don't works
        z: menuMouseArea.z + 1.
        transformOrigin: Item.Center
        spacing: 5
    }

    states: [
        State {
            name: "ACTIVATED"
            PropertyChanges { target: menu; width : 80; height: menuHeight; visible: true; opacity : 1.0 }
            StateChangeScript {
                script: {
                }
            }
        },
        State {
            name: "RELEASED"
            PropertyChanges { target: menu; width : 0; height: 20; visible : false; opacity : 0. }
            StateChangeScript {
                script: {
                    // When a menu is released, all its menu elements are also released exept the active one
                    for ( var i = 0; i < menuLayout.children.length; i++ )
                    {
                        if ( menuLayout.children[ i ].state !== "CHECKED" )     // Checked items should not be released...
                            menuLayout.children[ i ].state = "RELEASED";
                    }

                    // Parent element is released with this menu
                    if ( parentElement && parentElement.state != "CHECKED" )
                        parentElement.state = "RELEASED"
                }
            }
        },
        State {
            name: "ENVELOPED"
            PropertyChanges { target: menu; radius: 5; height: 80; width : menuHandler.width; visible : true; opacity : 0.1 }
            PropertyChanges { target: menuLayout; width : 0; visible : false; opacity : 0. }
            PropertyChanges { target: menuHandler; width: 25; height: 25 ; visible : true; opacity : 1 }
        }
    ]

    transitions: [
        Transition {
            from: "ACTIVATED"
            to: "RELEASED"
            NumberAnimation { target: menu; properties: "opacity"; duration: 500 }
        },
        Transition {
            from: "RELEASED"
            to: "ACTIVATED"
            NumberAnimation { target: menu; properties: "opacity, width, height"; duration: 300 }
        },
        Transition {
            from: "ENVELOPED"
            to: "ACTIVATED"
            NumberAnimation { target: menu; properties: "radius, opacity, width, height"; duration: 300 }
        },
        Transition {
            from: "ACTIVATED"
            to: "ENVELOPED"
            NumberAnimation { target: menu; properties: "radius, opacity, width, height"; duration: 500 }
        }
    ]

    function releaseAllMenuElement( except )
    {
        console.log( "QanMenu::releaseAllMenuElement(): except " + except + " / " + except.label );
        for ( var i = 0; i < menuLayout.children.length; i++ ) {
            if ( menuLayout.children[ i ] !== except && menuLayout.children[ i ].state !== "CHECKED" )
                menuLayout.children[ i ].state = "RELEASED";
        }
    }
}

