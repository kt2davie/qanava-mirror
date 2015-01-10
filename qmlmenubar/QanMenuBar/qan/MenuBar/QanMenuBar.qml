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
// \file	QanMenuBar.qml
// \author	benoit@qanava.org
// \date	2015 January 09
//-----------------------------------------------------------------------------

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1

Rectangle {
    id: root
    width: 500
    height: 700
    color: "#00000000"
    radius: 0
    opacity: 0.8
    visible: true
    border.width: 0

    // Menu bar style sheet
    property QanMenuStyle style : QanMenuStyle { }

    /* Add a menu element with the given parameters in the sub menu of a given parent action. If no
       such sub menu exists for the given action, sub menu will be created. */
    function addMenuElement( parentMenuElement, elementLabel, elementImageSource, isChecked, action )
    {
        // Get the submenu attached to the parent action (and create it if no such sub menu exists)
        var elementMenu = null;
        if ( parentMenuElement )
        {
            elementMenu = parentMenuElement.childMenu;
            if ( !elementMenu ) // Parent menu element still does not have a child menu, lets create it
            {
                var component = Qt.createComponent( "QanMenu.qml" );
                elementMenu = component.createObject( parentMenuElement, { style: root.style, menuHeight: 180, label: parentMenuElement.label } );
                if ( elementMenu )
                {
                    elementMenu.parent = parentMenuElement;
                    elementMenu.label = parentMenuElement.label;
                    elementMenu.parentElement = parentMenuElement;
                    parentMenuElement.childMenu = elementMenu;
                }
            }
        }
        if ( !elementMenu ) // No parent menu element specified, use root menu bar menu
            elementMenu =  mainMenu;

        // Create the menu element and add it to the correct menu
        if ( elementMenu )
        {
            var component = Qt.createComponent( "QanMenuElement.qml" );
            var menuElement = component.createObject( mainMenu, { style: root.style, source: elementImageSource, label: elementLabel } );
            if ( menuElement )
            {
                menuElement.parent = elementMenu.layout;
                menuElement.parentMenu = elementMenu;
                elementMenu.elementCount++;
                if ( isChecked )
                    menuElement.state = "CHECKED";
                menuElement.action = action;
            }
        }
        return menuElement;
    }


    /* Insert a separator in this menu bar
        \note Be carefull, a separator cannot be add in an element specified with parentMenuElement if a
        child menu for this element has not already been created (ie you can't set a separator in an empty menu).
    */
    function addSeparator( parentMenuElement )
    {
        var elementMenu = null;
        if ( parentMenuElement )
            elementMenu = parentMenuElement.childMenu;
        if ( !elementMenu ) // No parent menu element specified, use root menu bar menu
            elementMenu =  mainMenu;

        var component;
        component = Qt.createComponent( "QanMenuSeparator.qml" );
        var menuSeparator = component.createObject( mainMenu );
        mainMenu.separatorCount++;
        if ( menuSeparator )
        {
            menuSeparator.parent = elementMenu.layout;
            menuSeparator.style = root.style;
        }
    }

    // Menu mouse area used to release the menu when the mouse exits
    MouseArea {
        id: menuMouseArea
        opacity: 1
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onEntered: {
            if ( mainMenu.state !== "ENVELOPED" )
                mainMenu.state = "ENVELOPED"
        }
    }

    QanMenu{
        id : mainMenu
        x : 0.
        y : 0.
        visible: true
        root: true
        label: "HLG"
        state: "ENVELOPED"
        Layout.fillHeight: true
        style : root.style

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }
}

