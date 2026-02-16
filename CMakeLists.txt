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
    src/geometry/trackgeometry.h
    src/controllers/ArcBridge.h
    src/controllers/JsonPointsLoader.h
    src/controllers/TrackController.h
    src/geometry/straightline.h
    src/geometry/straightline.cpp
    src/controllers/LineController.h
    src/controllers/LineBridge.h
)

target_include_directories(PlanetWasm PRIVATE src src/geometry)

qt_add_qml_module(PlanetWasm
    URI PlanetWasmApp
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/components/arc.qml
        qml/components/line.qml
        qml/components/track.qml
        qml/components/MainPanel.qml
        qml/components/AddArcPanel.qml
        qml/components/AddLinePanel.qml
        qml/components/PointImportPanel.qml
    RESOURCES
        assets/textures/earth.jpg
    NO_RESOURCE_TARGET_PATH
)

# Post-build: run wasm-opt on release builds
if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "Release")
    find_program(WASM_OPT wasm-opt)
    if(WASM_OPT)
        add_custom_command(TARGET PlanetWasm POST_BUILD
            COMMAND ${WASM_OPT} -Oz
                --enable-bulk-memory
                --enable-nontrapping-float-to-int
                --duplicate-function-elimination
                --merge-similar-functions
                --strip-producers
                --strip-debug
                --vacuum
                "$<TARGET_FILE_DIR:PlanetWasm>/PlanetWasm.wasm"
                -o "$<TARGET_FILE_DIR:PlanetWasm>/PlanetWasm.wasm"
            COMMENT "Running wasm-opt -Oz on PlanetWasm.wasm"
        )
    else()
        message(WARNING "wasm-opt not found, skipping WASM optimization")
    endif()
endif()

target_link_libraries(PlanetWasm PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Quick3D
)
