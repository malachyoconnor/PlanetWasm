import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    Layout.fillWidth: true
    spacing: 8

    Label {
        text: "Add Line"
        font.pixelSize: 16
        font.bold: true
        color: "white"
    }

    Label { text: "From"; color: "#aaa"; font.pixelSize: 12 }
    RowLayout {
        spacing: 8
        TextField {
            id: x1
            placeholderText: "X"
            text: "0"
            Layout.fillWidth: true
            color: "white"
            placeholderTextColor: "#666"
            background: Rectangle { color: "#2a2a3e"; radius: 4 }
        }
        TextField {
            id: y1
            placeholderText: "Y"
            text: "0"
            Layout.fillWidth: true
            color: "white"
            placeholderTextColor: "#666"
            background: Rectangle { color: "#2a2a3e"; radius: 4 }
        }
        TextField {
            id: z1
            placeholderText: "Z"
            text: "0"
            Layout.fillWidth: true
            color: "white"
            placeholderTextColor: "#666"
            background: Rectangle { color: "#2a2a3e"; radius: 4 }
        }
    }

    Label { text: "To"; color: "#aaa"; font.pixelSize: 12 }
    RowLayout {
        spacing: 8
        TextField {
            id: x2
            placeholderText: "X"
            text: "0.5"
            Layout.fillWidth: true
            color: "white"
            placeholderTextColor: "#666"
            background: Rectangle { color: "#2a2a3e"; radius: 4 }
        }
        TextField {
            id: y2
            placeholderText: "Y"
            text: "0"
            Layout.fillWidth: true
            color: "white"
            placeholderTextColor: "#666"
            background: Rectangle { color: "#2a2a3e"; radius: 4 }
        }
        TextField {
            id: z2
            placeholderText: "Z"
            text: "0"
            Layout.fillWidth: true
            color: "white"
            placeholderTextColor: "#666"
            background: Rectangle { color: "#2a2a3e"; radius: 4 }
        }
    }

    Label { text: "Color"; color: "#aaa"; font.pixelSize: 12 }
    RowLayout {
        spacing: 8
        Rectangle {
            width: 24; height: 24
            radius: 4
            color: lineColorField.text.length > 0 ? lineColorField.text : "#6bff6b"
        }
        TextField {
            id: lineColorField
            text: "#6bff6b"
            Layout.fillWidth: true
            color: "white"
            placeholderTextColor: "#666"
            background: Rectangle { color: "#2a2a3e"; radius: 4 }
        }
    }

    Button {
        text: "Add"
        Layout.fillWidth: true
        onClicked: {
            lineBridge.addLineFromUI(
                parseFloat(x1.text),
                parseFloat(y1.text),
                parseFloat(z1.text),
                parseFloat(x2.text),
                parseFloat(y2.text),
                parseFloat(z2.text),
                lineColorField.text
            )
        }
        background: Rectangle {
            color: parent.down ? "#5a5a8e" : "#4a4a7e"
            radius: 4
        }
        contentItem: Text {
            text: parent.text
            color: "white"
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
