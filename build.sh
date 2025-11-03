#!/bin/bash
set -e

COSMO_DIR="${COSMO_DIR:-/opt/cosmo}"
CC="${COSMO_DIR}/bin/cosmocc"
CFLAGS="-O2 -Wall -Wextra -std=c11 -Isrc -Ithird_party/sqlite3"
LDFLAGS="-static"
TARGET="app.com"
OBJ_DIR="obj"
SRC_DIR="src"
SQLITE3_DIR="third_party/sqlite3"

echo "=== Cosmopolitan Build Script ==="
echo "Toolchain: ${COSMO_DIR}"
echo "Target: ${TARGET}"
echo ""

if [ ! -d "${COSMO_DIR}" ]; then
    echo "Error: Cosmopolitan toolchain not found at ${COSMO_DIR}"
    echo "Please install Cosmopolitan or set COSMO_DIR environment variable"
    exit 1
fi

mkdir -p "${OBJ_DIR}"

SOURCES=(
    main.c
    http.c
    router.c
    db.c
    render.c
    admin.c
    board.c
    upload.c
)

OBJECTS=()

echo "Compiling SQLite3..."
"${CC}" ${CFLAGS} -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION \
    -c "${SQLITE3_DIR}/sqlite3.c" -o "${OBJ_DIR}/sqlite3.o"
OBJECTS+=("${OBJ_DIR}/sqlite3.o")

for source in "${SOURCES[@]}"; do
    obj_file="${OBJ_DIR}/${source%.c}.o"
    src_file="${SRC_DIR}/${source}"
    
    echo "Compiling ${src_file}..."
    "${CC}" ${CFLAGS} -c "${src_file}" -o "${obj_file}"
    OBJECTS+=("${obj_file}")
done

echo ""
echo "Linking ${TARGET}..."
"${CC}" ${CFLAGS} ${LDFLAGS} "${OBJECTS[@]}" -o "${TARGET}"

echo ""
echo "Build complete: ${TARGET}"
echo "Run with: ./${TARGET}"
