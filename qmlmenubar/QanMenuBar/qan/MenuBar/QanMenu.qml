/*
    Copyright (C) 2008-2015 Benoit AUTHEMAN

    This file is part of Qanava.

    Qanava is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Qanava is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Qanava.  If not, see <http://www.gnu.org/licenses/>.
*/

//-----------------------------------------------------------------------------
// This file is a part of the Qanava software.
//
// \file	QanMenu.qml
// \author	benoit@qanava.org
// \date	2015 January 09
//-----------------------------------------------------------------------------

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1


Rectangle {
    id: menu
    state: "RELEASED"

    // Menu bar style sheet
    property QanMenuStyle style

    x: parent.x + parent.width
    y: parent.height / 5
    width: 80
    height: ( elementCount * style.element.height + separatorCount * 10 > 80 ? elementCount * style.element.height + separatorCount * 10 : 80 );

    // Public properties //------------

    // Visual configuration
    radius: style.menu.border.radius
    border.width: style.menu.border.width
    border.color: style.menu.border.color
    gradient: style.menu.gradient
    visible: false
    opacity: 1.

    // Used internally to generate a mask for an associed QQuickWidget, and cache values
    property real   menuX: x
    property real   menuY: y
    property real   menuWidth: width
    property real   menuHeight: ( elementCount * style.element.height + separatorCount * 10 > 80 ? elementCount * style.element.height + separatorCount * 10 : 80 )

    // Set to true if the menu is not a root menu
    property bool   root: false

    // Menu label
    property string label: ""

    // Menu parent element (can be null)
    property QanMenuElement parentElement

    // Main menu layout (child elements should affect this layout as their parent: childElement.parent = thisMenu.menuLayout.)
    property ColumnLayout layout: menuLayout

    // Private properties //-----------
    property int    elementCount: 0
    property int    separatorCount: 0

    // Menu mouse area used to release the menu when the mouse exits
    MouseArea {
        id: menuMouseArea
        opacity: 1
        anchors.fill: parent
        z: menu.z + 1.
        hoverEnabled: true
        propagateComposedEvents: true
        onEntered: {
            if ( style.debug )
                console.log( "QanMenu::MouseArea::onEntered(): " + menu + " / " + menu.label );
            console.log( "QanMenu::MouseArea::onEntered(): " + menu.x + " " + menu.y + " " + menu.width + " " + menu.height );
        }

        onExited: {
            style.expandedAtStartup = false;    // A menu shown at startup behave like a normal menu after it has been hovered,
                                                // so manually unset the expanded at startup flag
            if ( style.debug )
                console.log( "QanMenu::MouseArea::onExited(): " + menu + " / " + menu.label );
            if ( menu.root ) {   // For root menu, release all menu elements
                for ( var c = 0; c < menuLayout.children.length; c++ )
                    if ( menuLayout.children[ c ].state !== "CHECKED" )
                        menuLayout.children[ c ].state = "RELEASED" ;
                menu.state = "ENVELOPED";
            }
        }

        Image {
            id: menuHandler
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
                    if ( style.debug )
                        console.log( "QanMenu:menuHandlerMouseArea::onEntered()" );
                    if ( menu.state != "ACTIVATED" )
                        menu.state = "ACTIVATED"
                }
            }
        }

        // Layout must be a child of mouse area to prevent an exited() signal to be cast
        // when a child elemnt is entered
        ColumnLayout {
            id: menuLayout
            anchors.fill: parent
            anchors.margins: 7  // Must not be zero or mouse area onExited don't works
            z: menuMouseArea.z + 1.
            transformOrigin: Item.Center
            spacing: 5
        }
    }

    states: [
        State {
            name: "ACTIVATED"
            PropertyChanges { target: menu; width : 80; height: menuHeight; visible: true; color: "transparent"; opacity : 1.0 }
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
            PropertyChanges { target: menu; radius: 5; height: 80; width : menuHandler.width; visible : true; color: "lightgray"; opacity : 0.3 }
            PropertyChanges { target: menuLayout; width : 0; visible : false; opacity : 0. }
            PropertyChanges { target: menuHandler; width: 25; height: 25 ; visible : true; opacity : 1 }
        }
    ]

    transitions: [
        Transition {
            from: "ACTIVATED"
            to: "RELEASED"
            SequentialAnimation {
                ScriptAction {
                    script: {
                        var sceneX = mapXtoScene( menu );
                        var sceneY = mapYtoScene( menu );
                        qanMenuBar.removeBrFromMask( sceneX, sceneY - 5, menu.width, menu.height + 10 );
                    }
                }
                NumberAnimation { target: menu; properties: "opacity"; duration: 500 }
            }
        },
        Transition {
            from: "RELEASED"
            to: "ACTIVATED"
            SequentialAnimation {
                NumberAnimation { target: menu; properties: "opacity, width, height"; duration: 300 }
                ScriptAction {
                    script: {
                        var sceneX = mapXtoScene( menu );
                        var sceneY = mapYtoScene( menu );
                        if ( style.debug )
                            console.log( "RELEASED->ACTIVATED (scene) for " + menu.label + " : " + sceneX + " " + sceneY + " " + menu.width + " " + menu.height );
                        qanMenuBar.appendBrToMask( sceneX, sceneY - 5., menu.width, menu.menuHeight + 10 );
                    }
                }
            }
        },
        Transition {
            from: "ENVELOPED"
            to: "ACTIVATED"
            SequentialAnimation {
                NumberAnimation { target: menu; properties: "radius, opacity, width, height"; duration: 300 }
                ScriptAction {
                    script: {
                        var r = menu.mapToItem( null, menu.x, menu.y, menu.width, menu.menuHeight );
                        if ( style.debug )
                            console.log( "ENVELOPED->ACTIVATED (scene) for " + menu.label + " : " + r.x + " " + r.y + " " + r.width + " " + r.height );
                        qanMenuBar.resetMask( r.x, r.y - 5., r.width + 5., r.height + 10. );    // Add a 5 pixel margin to catch exited signals
                    }
                }
            }
        },
        Transition {
            from: "ACTIVATED"
            to: "ENVELOPED"
            SequentialAnimation {
                NumberAnimation { target: menu; properties: "radius, opacity, width, height"; duration: 500 }
                ScriptAction {
                    script: { qanMenuBar.resetMask( menuHandler.x, menuHandler.y, menuHandler.width, menuHandler.height ); }
                }
            }
        }
    ]

    function mapXtoScene( item )
    {
        return ( item.parent !== null ? item.x + mapXtoScene( item.parent ) : item.x );
    }

    function mapYtoScene( item )
    {
        return ( item.parent !== null ? item.y + mapYtoScene( item.parent ) : item.y );
    }

    function releaseAllMenuElement( except )
    {
        if ( style.debug )
            console.log( "QanMenu::releaseAllMenuElement(): except " + except + " / " + except.label );
        for ( var i = 0; i < menuLayout.children.length; i++ ) {
            if ( menuLayout.children[ i ] !== except && menuLayout.children[ i ].state !== "CHECKED" )
                menuLayout.children[ i ].state = "RELEASED";
        }
    }
}

