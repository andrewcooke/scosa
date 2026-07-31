#!/bin/bash -e

cd "$(dirname "$0")"

if [ ! -e build ]; then
    mkdir build
fi
cd build

set -x

#cmake --build . --target clean

# SC_PATH is now optional - will auto-detect from system installation
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Release --verbose
#ctest --output-on-failure
cmake --install .
