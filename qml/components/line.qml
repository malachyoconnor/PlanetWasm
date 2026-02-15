import QtQuick3D
import PlanetWasmApp
Model {
    id: arcModel
    geometry: StraightLine {  }
    materials: PrincipledMaterial {
        baseColor: "white"
        lighting: PrincipledMaterial.NoLighting
    }
}