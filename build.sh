#!/bin/bash -e

cd "$(dirname "$0")"

if [ ! -e build ]; then
    mkdir build
fi
cd build

set -x

#cmake --build . --target clean

cmake .. -DCMAKE_INSTALL_PREFIX=/home/andrew/.local/share/SuperCollider/Extensions/ -DSTRICT=ON
cmake --build . --config Release --verbose
cmake --install .
