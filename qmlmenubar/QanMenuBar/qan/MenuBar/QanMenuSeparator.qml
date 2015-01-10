
import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1

Item {
    id: menuSeparator
    objectName: "QanMenuSeparator"    // Do not change separator object name...

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
        // FIXME !!!!!!
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - 6
        height: 1

        onPaint: {
            var context = getContext( "2d" );
            context.beginPath( );
            context.lineWidth = 1;
            context.moveTo( 1, 0 );
            context.strokeStyle = "white";
            context.lineTo( width, 0 );
            context.stroke( );
        }
    }
}
