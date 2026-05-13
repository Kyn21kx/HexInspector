#!/usr/bin/env bash
set -euo pipefail

PROJECT="HexInspector"
SRC_DIR="src"
THIRD_PARTY="third_party"
BUILD_DIR="build_clang"
CLAY_LIB="$BUILD_DIR/libclay.a"
OUTPUT="$BUILD_DIR/$PROJECT"

C_FILES=("renderer/clay_renderer_raylib.c")
CPP_FILES=("main.cpp" "Application.cpp" "FileLayer.cpp")
INCLUDES=("-I$THIRD_PARTY/clay/include" "-I$THIRD_PARTY/raylib/include")
LIB_DIRS=("-L$THIRD_PARTY/raylib/lib")
LIBS=("-lraylib" "-lc")

CFLAGS=("-std=c99" "-DCLAY_IMPLEMENTATION" "-g")
CPPFLAGS=("-std=c++20" "-Wno-reorder" "-g")

OS=$(uname -s)

FRAMEWORKS=()
RPATHS=()
PLATFORM_LIBS=()

case "$OS" in
    Darwin)
        FRAMEWORKS=("-framework" "Cocoa" "-framework" "IOKit" "-framework" "CoreVideo")
        RPATHS=("-Wl,-rpath,@executable_path")
        ;;
esac

case "$OS" in
    MINGW*|MSYS*|CYGWIN*)
        PLATFORM_LIBS=("-lgdi32" "-luser32" "-lshell32")
        ;;
esac

mkdir -p "$BUILD_DIR"

echo "==> Compiling clay renderer (C)"
C_OBJECTS=()
for file in "${C_FILES[@]}"; do
    obj="$BUILD_DIR/$(basename "$file" .c).o"
    clang -c "$SRC_DIR/$file" -o "$obj" "${CFLAGS[@]}" "${INCLUDES[@]}"
    C_OBJECTS+=("$obj")
done

echo "==> Creating clay static library"
ar rcs "$CLAY_LIB" "${C_OBJECTS[@]}"

echo "==> Compiling C++ sources"
CPP_OBJECTS=()
for file in "${CPP_FILES[@]}"; do
    obj="$BUILD_DIR/$(basename "$file" .cpp).o"
    clang++ -c "$SRC_DIR/$file" -o "$obj" "${CPPFLAGS[@]}" "${INCLUDES[@]}"
    CPP_OBJECTS+=("$obj")
done

echo "==> Linking executable"
clang++ "${CPP_OBJECTS[@]}" "$CLAY_LIB" -o "$OUTPUT" \
    "${LIB_DIRS[@]}" "${LIBS[@]}" \
    ${RPATHS[@]+"${RPATHS[@]}"} \
    ${FRAMEWORKS[@]+"${FRAMEWORKS[@]}"} ${PLATFORM_LIBS[@]+"${PLATFORM_LIBS[@]}"}

case "$OS" in
    Darwin)
        echo "==> Bundling raylib dylib"
        cp "$THIRD_PARTY/raylib/lib/libraylib.550.dylib" "$BUILD_DIR/"
        ;;
esac

echo "==> Done: $OUTPUT"
echo "    Ship '$BUILD_DIR/$PROJECT' and '$BUILD_DIR/libraylib.550.dylib' together"
