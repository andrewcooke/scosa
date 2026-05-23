# SCosa

Author: andrew cooke

Euclidean Rhythm Generator

### Requirements

- CMake >= 3.5
- SuperCollider source code

### Building

Clone the project:

    git clone https://github.com/andrewcooke/scosa
    cd scosa
    mkdir build
    cd build

Then, use CMake to configure and build it:

    cmake .. -DCMAKE_INSTALL_PREFIX=/home/andrew/.local/share/SuperCollider/Extensions/
    cmake --build . --config Release
    cmake --install .

It's expected that the SuperCollider repo is cloned at `../supercollider` relative to this repo. If
it's not: add the option `-DSC_PATH=/path/to/sc/source`.

### Developing

Use the command in `regenerate` to update CMakeLists.txt when you add or remove files from the
project. You don't need to run it if you only change the contents of existing files. You may need to
edit the command if you add, remove, or rename plugins, to match the new plugin paths. Run the
script with `--help` to see all available options.
