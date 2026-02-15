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
