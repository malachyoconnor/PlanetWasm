#!/usr/bin/env bash
set -euo pipefail

REQUIRED_EMCC="4.0.7"

error_exit() {
    echo -e "Error: $1" >&2
    echo -e "Fix: $2" >&2
    exit 1
}

check_environment() {
    # Check if emcc exists
    if ! command -v emcc &>/dev/null; then
        if [[ -n "${EMSDK:-}" ]]; then
            error_exit "emcc not found, but \$EMSDK is set ($EMSDK)." \
                       "source \$EMSDK/emsdk_env.sh"
        else
            error_exit "emcc not found." \
                       "Activate emsdk first: source <path-to-emsdk>/emsdk_env.sh"
        fi
    fi

    # Check version
    local current_version=$(emcc --version | grep -oP '\d+\.\d+\.\d+' | head -1)
    if [[ "$current_version" != "$REQUIRED_EMCC" ]]; then
        error_exit "Emscripten $REQUIRED_EMCC required (found $current_version)." \
                   "Run: emsdk activate $REQUIRED_EMCC && source emsdk_env.sh"
    fi
}

check_environment

echo "Building WASM project..."
cmake --preset wasm
cmake --build --preset wasm --parallel

echo -e "\nBuild complete. Serve with:"
echo "python3 -m http.server -d build-wasm/"