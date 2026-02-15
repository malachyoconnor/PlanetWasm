import QtQuick3D
import PlanetWasmApp
Model {
    id: arcModel
    property real cameraZ: parent ? parent.cameraZ : 1.0
    geometry: ArcGeometry { cameraZ: arcModel.cameraZ }
    materials: PrincipledMaterial {
        baseColor: "white"
        lighting: PrincipledMaterial.NoLighting
    }
}