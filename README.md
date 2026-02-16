# PlanetWasm

An interactive 3D globe built with C++20 and Qt Quick 3D that visualizes location data as arcs and tracks on an Earth sphere. Compiles to both native desktop and WebAssembly.

## Features

- 3D Earth with texture mapping and orbit camera controls (drag to rotate, scroll to zoom)
- Import location data from JSON and render tracks on the globe
- Add arcs between points on the sphere
- Runs natively or in the browser via WebAssembly

## Building

### Native (Desktop)

Requires Qt 6.10+ with Quick3D.

```bash
cmake --preset native
cmake --build --preset native
./build-native/PlanetWasm
```

### WebAssembly

Requires Emscripten 4.0.7 and Qt 6 built for WASM (e.g. `qt6-wasm` AUR package at `/opt/qt6-wasm`).

```bash
# Activate Emscripten
cd ~/Packages/emsdk && ./emsdk activate 4.0.7 && source ./emsdk_env.sh

# Build
cd /path/to/PlanetWasm
./build-wasm.sh

# Serve locally
python3 serve-wasm.py
# Open http://localhost:8000/PlanetWasm.html
```

## Tech Stack

- **C++20** with CMake
- **Qt 6.10+** — Core, Gui, Qml, Quick, Quick3D
- **Emscripten** — WebAssembly compilation