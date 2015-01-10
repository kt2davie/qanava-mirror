import QtQuick 2.0


// Define a QanMenu stylesheet with default properties
QtObject {
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

