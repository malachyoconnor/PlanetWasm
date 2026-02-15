import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


ColumnLayout {
    Layout.fillWidth: true
    spacing: 8

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
