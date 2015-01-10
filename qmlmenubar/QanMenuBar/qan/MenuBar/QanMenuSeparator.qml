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
// \file	QanMenuSeparator.qml
// \author	benoit@qanava.org
// \date	2015 January 09
//-----------------------------------------------------------------------------

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1

Item {
    id: menuSeparator
    objectName: "QanMenuSeparator"    // Do not change separator object name...

    // Menu bar style sheet (initialized by QanMenuBar)
    property QanMenuStyle style

    anchors.margins: 8
    Layout.alignment: Qt.AlignCenter
    Layout.fillWidth : true
    Layout.fillHeight : false

    Layout.preferredWidth: 32
    Layout.preferredHeight: 5

    Layout.minimumWidth: 16
    Layout.minimumHeight: 2

    Layout.maximumWidth: Number.POSITIVE_INFINITY
    Layout.maximumHeight: 5

    opacity: 0.7

    // Define a second rectangle to ensure that its height is 1 (cant't define an height in parent while using quick layout)
    Canvas {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - 6
        height: 1

        onPaint: {
            var context = getContext( "2d" );
            context.beginPath( );
            context.lineWidth = 1;
            context.moveTo( 1, 0 );
            context.strokeStyle = parent.style.separatorColor;
            context.lineTo( width, 0 );
            context.stroke( );
        }
    }
}
