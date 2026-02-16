import QtQuick3D
import PlanetWasmApp
Model {
    id: trackModel
    geometry: TrackGeometry { }
    materials: PrincipledMaterial {
        baseColor: "white"
        lighting: PrincipledMaterial.NoLighting
    }
}
