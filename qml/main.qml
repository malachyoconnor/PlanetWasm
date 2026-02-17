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
            property real cameraZ: camera.position.z
        }

        anchors.fill: parent

        environment: SceneEnvironment {
            clearColor: "#1e1e2e"
            backgroundMode: SceneEnvironment.Color
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

        Model {
            id: earth
            source: "#Sphere"
            position: Qt.vector3d(0, 0, 0)
            // For some reason - when set to 0.01, the sphere is very slightly too small!!
            scale: Qt.vector3d(0.010110, 0.010110, 0.010110)

            materials: [
                DefaultMaterial {
                    diffuseMap: Texture {
                        source: "qrc:/assets/textures/earth.jpg"
                    }
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

    MainPanel {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 16
    }

    Rectangle {
        id: infoBox
        x: 16
        y: -height
        width: 320
        color: "#cc1e1e2e"
        radius: 8
        height: infoColumn.height + 24

        Component.onCompleted: y = 16

        Behavior on y {
            NumberAnimation {
                duration: 1000
                easing.type: Easing.OutCubic
            }
        }

        Column {
            id: infoColumn
            anchors.left: parent.left
            anchors.right: closeButton.left
            anchors.top: parent.top
            anchors.margins: 13
            spacing: 6

            Text {
                text: "PlanetWasm \u2708"
                font.pixelSize: 18
                font.bold: true
                color: "#cdd6f4"
            }

            Text {
                text: "Visualise your travel history on a 3D globe.\nDrag to orbit, right click to look around and scroll to zoom."
                font.pixelSize: 16
                color: "#a6adc8"
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }

        Text {
            id: closeButton
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 8
            text: "\u2715"
            font.pixelSize: 14
            color: "#a6adc8"

            MouseArea {
                anchors.fill: parent
                anchors.margins: -4
                cursorShape: Qt.PointingHandCursor
                onClicked: infoBox.visible = false
            }
        }
    }
}
