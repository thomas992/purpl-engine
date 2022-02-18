# Purpl Engine
[![CI](https://github.com/MobSlicer152/purpl-engine/actions/workflows/main.yml/badge.svg)](https://github.com/MobSlicer152/purpl-engine/actions/workflows/main.yml)

This is my fifth game engine, it's gonna be good this time

## Building the engine

All the tools needed to build the engine (other than the compiler and CMake) is provided, and licenses for the libraries and programs are in `deps/licenses`.

On Windows, you need Visual Studio 2019 16.8 or later (i.e. C17 support. However, earlier versions may work) and the Windows SDK (any recentish one should be fine, but for best results use the latest Windows 11 one {which even works on Windows 7, just as proof of how backwards compatible they are}). On Linux, you need the X11 or Wayland development headers and any version of Clang supporting C2x or later (should be fairly easy to get) or a version of GCC supporting `[[fallthrough]]` in C mode (SDL uses it, and GCC 9.3 doesn't support it). You also need CMake. In addition, on Windows and Linux, you have to install the Vulkan SDK (use the installer or your distro's package).

Run CMake like so:
```shell
# GCC 9 (Ubuntu's default) doesn't seem to be able to compile SDL right now, so you may need to append `-DCMAKE_C_COMPILER=clang` if you have an older GCC
cmake -S. -Bbuild
cmake --build build
cmake --install build # Installs to `build/install`
```
