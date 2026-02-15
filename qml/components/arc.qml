import QtQuick3D
import PlanetWasmApp
Model {
    geometry: ArcGeometry {}
    materials: PrincipledMaterial {
        baseColor: "white"
        lighting: PrincipledMaterial.NoLighting
    }
}