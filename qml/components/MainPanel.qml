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

        Button {
            id: exampleBtn
            Layout.fillWidth: true
            property int nextPath: 1
            property var labels: ["Load Example (Europe)", "Load Example (Asia)", "Load Example (Americas)"]
            text: labels[nextPath - 1]
            enabled: nextPath <= 3
            onClicked: {
                pointsLoader.loadResource(":/assets/examplePath" + nextPath + ".json")
                nextPath++
                if (nextPath > 3)
                    text = "All Examples Loaded"
            }

            background: Rectangle {
                color: !exampleBtn.enabled ? "#222" : (exampleBtn.down ? "#4a4a6e" : (exampleBtn.hovered ? "#3a3a5e" : "#2a2a3e"))
                radius: 6
                border.color: "#555"
                border.width: 1
            }
            contentItem: Text {
                text: exampleBtn.text
                color: exampleBtn.enabled ? "white" : "#666"
                font.pixelSize: 13
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#444"
        }

        AddArcPanel {}

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#444"
        }

        // Useful panel for debugging! Drawing straight lines can be handy
        // AddLinePanel {}
        // Rectangle {
        //     Layout.fillWidth: true
        //     height: 1
        //     color: "#444"
        // }

        PointImportPanel {}
    }
}
