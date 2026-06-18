#!/bin/bash -e

cd "$(dirname "$0")"

if [ ! -e build ]; then
    mkdir build
fi
cd build

set -x

#cmake --build . --target clean

#cmake .. -DSC_PATH=/home/andrew/pkg/supercollider -DCMAKE_INSTALL_PREFIX=/home/andrew/.local/share/SuperCollider/Extensions/ -DSTRICT=ON
cmake .. -DSC_PATH=/home/andrew/pkg/supercollider -DCMAKE_INSTALL_PREFIX=/home/andrew/.local/share/SuperCollider/Extensions/
cmake --build . --config Release --verbose
ctest --output-on-failure
cmake --install .
