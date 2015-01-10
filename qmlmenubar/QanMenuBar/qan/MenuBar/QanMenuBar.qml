
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

    // Look for an element associed to a given action in a specific menu (return null if no such action has been registered in this menu bar)
    // FIXME: move this method to QanMenu, will end up having only one parameter ?
    // FIXME: children class name must be verified for QanMenuElement....
    // FIXME: no longer necessary...
    function    findMenuElement( menu, action )
    {
        var element;
        if ( !action )
            return element;

        for ( var c = 0; c < menu.menuLayout.children.length; c++ )
        {
            var menuElement = menu.menuLayout.children[ c ];
            console.log( "QanMenuBar::findMenuElement(): " + menuElement.className )

            if ( menuElement.action === action )
                return menuElement
        }
        return element;
    }

    /* Add a menu element with the given parameters in the sub menu of a given parent action. If no
       such sub menu exists for the given action, sub menu will be created. */
    function addMenuElement( parentMenuElement, elementLabel, elementImageSource, isChecked, action )
    {
        if ( parentMenuElement )
            console.log( "QanMenuBar::addMenuElement() '" + elementLabel + "' with parent element: '" + parentMenuElement.label + "'" );

        // Get the submenu attached to the parent action (and create it if no such sub menu exists)
        var elementMenu = null;
        if ( parentMenuElement )
        {
            elementMenu = parentMenuElement.childMenu;
            if ( !elementMenu ) // Parent menu element still does not have a child menu, lets create it
            {
                var component = Qt.createComponent( "QanMenu.qml" );
                elementMenu = component.createObject( parentMenuElement, { menuHeight: 180, label: parentMenuElement.label } );
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
            var menuElement = component.createObject( mainMenu, { source: elementImageSource, label: elementLabel } );
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
        console.log( "QanMenuBar::addSeparator(): " );

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
            menuSeparator.parent = elementMenu.layout;
    }

    // Menu mouse area used to release the menu when the mouse exits
    MouseArea {
        id: menuMouseArea
        opacity: 1
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onEntered: {
            console.log( "menu::MouseArea::onEntered(): " );
            if ( mainMenu.state !== "ENVELOPED" )
                mainMenu.state = "ENVELOPED"
        }
    }

    QanMenu{
        id : mainMenu
        //menuHeight: 300
        x : 0.
        y : 0.
        visible: true
        root: true
        label: "HLG"
        state: "ENVELOPED"
        Layout.fillHeight: true

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

 /*       QanMenuElement {
            id: fileMenuElement
            source: "qrc:/images/oxygen/document-open-folder.svg"
            parentMenu: mainMenu
            childMenu: fileMenu
            label: "File"

            QanMenu {
                id: fileMenu
                menuHeight: 180
                label: "File"
                parentElement: fileMenuElement

                QanMenuElement {
                    id: fileMenuOpenElement
                    source: "qrc:/images/oxygen/document-open.svg"
                    parentMenu: fileMenu
                    label: "Open"
                }
                QanMenuElement {
                    id: fileMenuSaveAsElement
                    source: "qrc:/images/oxygen/document-save-as.svg"
                    parentMenu: fileMenu
                    label: "Save As"
                }
                QanMenuElement {
                    id: fileMenuExitElement
                    source: "qrc:/images/oxygen/application-exit.svg"
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

        QanMenuElement{
            id: topologyMenuElement
            source: "qrc:/images/oxygen/code-class.svg"
            label: "Topology"
            parentMenu: mainMenu
            childMenu: topologyMenu

            QanMenu {
                id: topologyMenu
                menuHeight: 180
                label: "Topology"
                parentElement: topologyMenuElement

                QanMenuElement {
                    id: topologyMenuAddTagElement
                    source: "qrc:/images/oxygen/insert-text.svg"
                    label: "Add Tag"
                    parentMenu: topologyMenu
                }
                QanMenuElement {
                    id: topologyMenuAddLinkElement
                    source: "qrc:/images/oxygen/insert-link.svg"
                    label: "Add Link"
                    parentMenu: topologyMenu
                }
                QanMenuElement {
                    id: topologyMenuAddhLinkElement
                    source: "qrc:/images/oxygen/insert-link.svg"
                    label: "Add hLink"
                    parentMenu: topologyMenu
                }
                Component.onCompleted: {
                    // Reparent menu element to this menu layout
                    topologyMenuAddTagElement.parent = topologyMenu.layout
                    topologyMenuAddLinkElement.parent = topologyMenu.layout
                    topologyMenuAddhLinkElement.parent = topologyMenu.layout
                }
            }
        }

        QanMenuElement{
            id: groupsMenuElement
            source: "qrc:/images/oxygen/preferences-plugin.svg"
            label: "Groups"
            parentMenu: mainMenu
            childMenu: groupsMenu

            QanMenu {
                id: groupsMenu
                menuHeight: 180
                label: "Topology"
                parentElement: groupsMenuElement

                QanMenuElement {
                    id: groupsMenuAddSequence
                    source: "qrc:/images/oxygen/view-time-schedule.svg"
                    label: "Sequence"
                    parentMenu: groupsMenu
                }
                QanMenuElement {
                    id: groupsMenuAddProspective
                    source: "qrc:/images/oxygen/split.svg"
                    label: "Prospective"
                    parentMenu: groupsMenu
                }
                QanMenuElement {
                    id: groupsMenuAddRelational
                    source: "qrc:/images/oxygen/office-chart-polar.svg"
                    label: "Relational"
                    parentMenu: groupsMenu
                }
                Component.onCompleted: {
                    // Reparent menu element to this menu layout
                    groupsMenuAddSequence.parent = groupsMenu.layout
                    groupsMenuAddProspective.parent = groupsMenu.layout
                    groupsMenuAddRelational.parent = groupsMenu.layout
                }
            }
        }*/

       /* QanMenuSeparator { id: separator }

        QanMenuElement{
            id: editStylesMenuElement
            source: "qrc:/images/oxygen/media-playlist.svg"
            label: "Edit Styles"
            parentMenu: mainMenu
        }
        QanMenuElement{
            id: showStyleBrowserMenuElement
            source: "qrc:/images/oxygen/office-chart-ring.svg"
            label: "Style \nBrowser"
            parentMenu: mainMenu
        }*/

        Component.onCompleted: {
            // Reparent menu element to this menu layout
            /*fileMenuElement.parent = mainMenu.layout
            topologyMenuElement.parent = mainMenu.layout
            groupsMenuElement.parent = mainMenu.layout
            separator.parent = mainMenu.layout*/
            /*editStylesMenuElement.parent = mainMenu.layout
            showStyleBrowserMenuElement.parent = mainMenu.layout
            mainMenu.elementCount += 2;*/
       }
    }
}

