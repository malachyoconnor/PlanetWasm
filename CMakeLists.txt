cmake_minimum_required(VERSION 3.24)
project(PlanetWasm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Quick3D)
qt_standard_project_setup()

qt_add_executable(PlanetWasm
    src/main.cpp
    src/geometry/arcgeometry.h
    src/geometry/arcgeometry.cpp
    src/controllers/ArcBridge.h
)

target_include_directories(PlanetWasm PRIVATE src src/geometry)

qt_add_qml_module(PlanetWasm
    URI PlanetWasmApp
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/components/arc.qml
        qml/components/ArcPanel.qml
    RESOURCES assets/textures/earth.jpg
    NO_RESOURCE_TARGET_PATH
)

target_link_libraries(PlanetWasm PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Quick3D
)
