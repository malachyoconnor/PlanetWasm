import QtQuick3D
import PlanetWasmApp
Model {
    id: trackModel
    property real cameraZ: parent ? parent.cameraZ : 1.0
    geometry: TrackGeometry { cameraZ: trackModel.cameraZ }
    materials: PrincipledMaterial {
        baseColor: "white"
        lighting: PrincipledMaterial.NoLighting
    }
}
