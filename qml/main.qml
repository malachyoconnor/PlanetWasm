import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
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
            property real cameraZ: camera.position.z
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
            eulerRotation.x: -54
            eulerRotation.y: 2
            PerspectiveCamera {
                id: camera
                clipNear: 0.0001
                clipFar: 100
                position: Qt.vector3d(0, 0, 1)
            }
        }

        DirectionalLight {
            id: light
            position: Qt.vector3d(0, 0, 0.67)
            eulerRotation.x: -30
            eulerRotation.y: 30
        }

        Texture {
            id: earthTexture
            source: "qrc:/assets/textures/earth.jpg"
        }

        SphereGeometry {
            id: earthGeometry
            radius: 0.5
            rings: 64
            segments: 512
        }

        Model {
            id: earth
            geometry: earthGeometry
            eulerRotation.y: 90

            scale: Qt.vector3d(1, 1, 1)

            materials: [
                PrincipledMaterial {
                    baseColorMap: earthTexture
                    roughness: 0.7
                    metalness: 0.05
                }
            ]
        }

    }

    // Made a MouseArea to capture scroll events
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: (wheel) => {
            let zoomSpeed = 0.00033 * (camera.position.z - 0.5) / 0.5;
            let z = camera.position.z - wheel.angleDelta.y * zoomSpeed;
            camera.position.z = Math.min(Math.max(z, 0.502), 2.67);
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
                    let speed = 0.3 * (camera.position.z - 0.5) / 0.5;
                    let dx = centroid.position.x - lastPos.x;
                    let dy = centroid.position.y - lastPos.y;
                    cameraOrbit.eulerRotation.y -= dx * speed;
                    cameraOrbit.eulerRotation.x -= dy * speed;
                    light.eulerRotation.y -= dx * speed;
                    light.eulerRotation.x -= dy * speed;
                    lastPos = centroid.position;
                }
            }
        }

        DragHandler {
            target: null
            acceptedButtons: Qt.RightButton
            property point lastPos
            onActiveChanged: {
                if (active)
                    lastPos = centroid.position;
            }
            onCentroidChanged: {
                if (active) {
                    let dx = centroid.position.x - lastPos.x;
                    let dy = centroid.position.y - lastPos.y;
                    camera.eulerRotation.y -= dx * 0.3;
                    camera.eulerRotation.x -= dy * 0.3;
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
