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
// \file	QanMenuElement.qml
// \author	benoit@qanava.org
// \date	2015 January 09
//-----------------------------------------------------------------------------

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import QtGraphicalEffects 1.0

Rectangle {
    id: menuElement
    state: "RELEASED"
    objectName: "QanMenuElement"    // Do not change menu element object name...

    // Menu bar style sheet (initialized by QanMenuBar)
    property QanMenuStyle style
    color: "transparent"
    opacity: 1.

    anchors.margins: 5
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

    // An eventual Qt action associed to this menu element.
    property QtObject   action;


    // Private properties //-----------
    property color hilightGradColor : style.element.hilightGradColor

    states: [
        State {
            name: "CHECKED"
            PropertyChanges { target: hiligther; opacity : 0.7; }
            PropertyChanges { target: menuElement; hilightGradColor : style.element.checkedGradColor ; scale : 1.1 }
        },
        State {
            name: "ACTIVATED"
            PropertyChanges { target: hiligther; opacity : 0.7 }
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
        Rectangle { // Add a transparent content rectangle, or drop sadow graphics effect fon't works
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth : true
            Layout.fillHeight : true

            Layout.preferredWidth: 32
            Layout.preferredHeight: 32

            Layout.minimumWidth: 16
            Layout.minimumHeight: 16

            Layout.maximumWidth: Number.POSITIVE_INFINITY
            Layout.maximumHeight: Number.POSITIVE_INFINITY

            color: "transparent"
            Image {
                id: menuElementImage
                anchors.fill: parent
                source: menuElement.source
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                smooth: true
            }
            DropShadow {
                anchors.fill: menuElementImage
                visible: style.showShadows
                horizontalOffset: 2
                verticalOffset: 2
                radius: 4.0
                samples: 16
                color: "#80000000"
                source: menuElementImage
                fast: true
            }
        }

        Rectangle { // Add a transparent content rectangle, or drop sadow graphics effect fon't works
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: menuElementLabel.contentWidth
            Layout.preferredHeight: menuElementLabel.contentHeight
            Layout.minimumWidth: 6
            Layout.minimumHeight: 5

            color: "transparent"

            Text {
                id: menuElementLabel
                anchors.fill: parent
                smooth: true
                text : menuElement.label
                color: menuElement.style.textColor
            }
            DropShadow {
                anchors.fill: menuElementLabel
                visible: style.showShadows
                horizontalOffset: 2
                verticalOffset: 2
                radius: 4.0
                samples: 16
                color: "#80000000"
                source: menuElementLabel
                fast: true
            }
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
                color: "#00000000";
            }
            GradientStop {
                position: 0.50;
                color: hilightGradColor;
            }
            GradientStop {
                position: 1.00;
                color: "#00000000";
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
                if ( style.debug )
                    console.log( "QanMenuElement::MouseArea::onEntered(): " + menuElement.label );

                if ( menuElement.state !== "CHECKED" )
                    menuElement.state = "ACTIVATED";
                if ( menuElement.state === "CHECKED" )
                    menuElement.hilightGradColor = style.element.hilightCheckedGradColor;
                if ( menuElement.state === "CHECKED" && parentMenu )
                    parentMenu.releaseAllMenuElement( menuElement );
            }
            onExited: {
                if ( style.debug )
                    console.log( "QanMenuElement::MouseArea::onExited(): " + menuElement.label );
                if ( menuElement.state === "CHECKED" )
                    menuElement.hilightGradColor = style.element.checkedGradColor;
                // Do not force the RELEASED mode since child and parent menus are "leaders" for releasing menu elements
            }
            onClicked: {
                menuElement.activated( );
                if ( menuElement.state !== "CHECKED" )
                    menuElement.state = "RELEASED";
            }
        }
    }

    // Emitted when the element is activated and the user click this element
    signal  activated( );

    // Called by an eventually connected Qt Action (could be used to detect a checked state change with checkable actions).
    function setChecked( checked )
    {
        checkable = true
        if ( parentMenu.style.debug )
            console.log( "QanMenuElement::setChecked(): " + label + " is " + checked );
        if ( checked && state !== "CHECKED" )
            state = "CHECKED";
        if ( state === "CHECKED" && !checked )
            state = "RELEASED";
    }
}
