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
// \file	QanMenuStyle.qml
// \author	benoit@qanava.org
// \date	2015 January 09
//-----------------------------------------------------------------------------


import QtQuick 2.0


// Define a QanMenu stylesheet with default properties
QtObject {

    property bool   debug: false;
    property bool   expandedAtStartup : false;
    property real   menuOpacity : 0.8
    property color  textColor : "white"
    property bool   showShadows : true

    property QtObject element : QtObject {
        id : elementStyle
        property int height: 70;

        property color hilightGradColor : "#4f4ae1"
        property color checkedGradColor : "#f971ee"
        property color hilightCheckedGradColor : "#ff50f1"
    }

    property color separatorColor : "white"

    property QtObject menu : QtObject {
        id: menuStyle
        property QtObject border : QtObject {
            property int width: 1;
            property color color: "black";
            property int radius: 15;
        }

        property Gradient gradient : Gradient {
            GradientStop {
                id: gradientStop1
                position: 0.02;
                color: "#9d9d9d";
            }
            GradientStop {
                position: 0.17;
                color: "#9d9d9d";
            }
            GradientStop {
                position: 0.36;
                color: "#808080";
            }
            GradientStop {
                position: 0.95;
                color: "#323232";
            }
        }
    }
}

