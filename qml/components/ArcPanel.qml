import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    width: 260
    height: col.implicitHeight + 32
    color: "#cc1e1e2e"
    radius: 8

    ColumnLayout {
        id: col
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Label {
            text: "Add Arc"
            font.pixelSize: 16
            font.bold: true
            color: "white"
        }

        Label { text: "From"; color: "#aaa"; font.pixelSize: 12 }
        RowLayout {
            spacing: 8
            TextField {
                id: lat1
                placeholderText: "Lat"
                Layout.fillWidth: true
                color: "white"
                placeholderTextColor: "#666"
                background: Rectangle { color: "#2a2a3e"; radius: 4 }
            }
            TextField {
                id: lon1
                placeholderText: "Lon"
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
                id: lat2
                placeholderText: "Lat"
                Layout.fillWidth: true
                color: "white"
                placeholderTextColor: "#666"
                background: Rectangle { color: "#2a2a3e"; radius: 4 }
            }
            TextField {
                id: lon2
                placeholderText: "Lon"
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
                color: colorField.text.length > 0 ? colorField.text : "#ff6b6b"
            }
            TextField {
                id: colorField
                text: "#ff6b6b"
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
                arcBridge.addArcFromUI(
                    parseFloat(lat1.text),
                    parseFloat(lon1.text),
                    parseFloat(lat2.text),
                    parseFloat(lon2.text),
                    colorField.text
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

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#444"
        }

        Label {
            text: "Import Points"
            font.pixelSize: 16
            font.bold: true
            color: "white"
        }

        Rectangle {
            Layout.fillWidth: true
            height: 80
            radius: 6
            color: dropArea.containsDrag ? "#3a3a5e" : "#2a2a3e"
            border.color: dropArea.containsDrag ? "#7c7cff" : "#555"
            border.width: dropArea.containsDrag ? 2 : 1

            Label {
                anchors.centerIn: parent
                text: "Drop JSON file here"
                color: dropArea.containsDrag ? "#aaaaff" : "#888"
                font.pixelSize: 12
            }

            DropArea {
                id: dropArea
                anchors.fill: parent
                keys: ["text/uri-list"]
                onDropped: (drop) => {
                    if (drop.urls.length > 0)
                        pointsLoader.loadFile(drop.urls[0])
                }
            }
        }
    }
}
