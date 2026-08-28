#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
core_prefix=${VIDEO2X_PREFIX:-"$project_dir/../video2x/dist/macos-$(uname -m)"}
build_dir=${VIDEO2X_QT_BUILD_DIR:-"$project_dir/build-macos"}
install_dir=${VIDEO2X_QT_INSTALL_DIR:-"$project_dir/dist/macos-$(uname -m)"}

if [ ! -f "$core_prefix/lib/libvideo2x.dylib" ]; then
    printf 'Video2X macOS core not found at %s\n' "$core_prefix" >&2
    printf 'Build the core first with ../video2x/scripts/build-macos.sh\n' >&2
    exit 1
fi

cmake -G Ninja -S "$project_dir" -B "$build_dir" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt;$core_prefix" \
    -DCMAKE_INSTALL_PREFIX="$install_dir" \
    -DCMAKE_INSTALL_RPATH="@executable_path/../Frameworks;/opt/homebrew/lib;$core_prefix/lib" \
    -DVIDEO2X_PREFIX="$core_prefix" \
    -DUSE_EXTERNAL_VIDEO2X=ON \
    -DUSE_EXTERNAL_SPDLOG=ON \
    -DVIDEO2X_ENABLE_NATIVE=ON

cmake --build "$build_dir" --parallel
cmake --install "$build_dir"

printf 'Video2X.app installed in %s\n' "$install_dir"
