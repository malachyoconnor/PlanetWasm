import QtQuick
import QtQuick3D
import PlanetWasmApp

Model {
    id: arcModel
    property real cameraZ: parent ? parent.cameraZ : 1.0
    geometry: ArcGeometry { cameraZ: arcModel.cameraZ }
    materials: CustomMaterial {
        property real time: 0.0
        property color baseColor: "white"

        cullMode: Material.NoCulling
        fragmentShader: "qrc:/qml/shaders/arc.frag"

        NumberAnimation on time {
            from: 0.0
            to: 1.0
            duration: 3000
            loops: Animation.Infinite
        }
    }
}
