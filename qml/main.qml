import QtQuick
import QtQuick3D
import "components"

Window {
    width: 1280
    height: 720
    visible: true
    title: "PlanetWasm"

    View3D {
        Node {
            id: sceneRoot
            objectName: "sceneRoot"
        }

        anchors.fill: parent

        environment: SceneEnvironment {
            clearColor: "#1e1e2e"
            backgroundMode: SceneEnvironment.Color
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
        }

        Node {
            id: cameraOrbit
            PerspectiveCamera {
                id: camera
                position: Qt.vector3d(0, 0, 300)
            }
        }

        DirectionalLight {
            id: light
            position: Qt.vector3d(0, 0, 200)
            eulerRotation.x: -30
            eulerRotation.y: 30
        }

        Texture {
            id: earthTexture
            source: "qrc:/assets/textures/earth.jpg"
        }

        Model {
            source: "#Sphere"
            scale: Qt.vector3d(3, 3, 3)
            materials: PrincipledMaterial {
                baseColorMap: earthTexture
                roughness: 0.7
                metalness: 0.05
            }
        }

    }

    // Made a MouseArea to capture scroll events
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: (wheel) => {
            let z = camera.position.z - wheel.angleDelta.y * 0.1;
            camera.position.z = Math.min(Math.max(z, 165), 800);
        }

        DragHandler {
            target: null
            property point lastPos
            onActiveChanged: {
                if (active)
                    lastPos = centroid.position;
            }
            onCentroidChanged: {
                if (active) {
                    let dx = centroid.position.x - lastPos.x;
                    let dy = centroid.position.y - lastPos.y;
                    cameraOrbit.eulerRotation.y -= dx * 0.3;
                    cameraOrbit.eulerRotation.x -= dy * 0.3;
                    light.eulerRotation.y -= dx * 0.3;
                    light.eulerRotation.x -= dy * 0.3;
                    lastPos = centroid.position;
                }
            }
        }
    }

    ArcPanel {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 16
    }
}
