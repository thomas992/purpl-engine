# Purpl Engine
[![Build Status](https://dev.azure.com/MobSlicer152/purpl-engine/_apis/build/status/MobSlicer152.purpl-engine?branchName=main)](https://dev.azure.com/MobSlicer152/purpl-engine/_build/latest?definitionId=1&branchName=main)

This is my fifth game engine, it's gonna be good this time

## Building the engine

Most of the tools needed to build the engine (other than the compiler and CMake) are provided. You may also need to run these commands on Unix-y platforms:
``` sh
chmod a+x tools/fixperms.sh
tools/fixperms.sh
```

On Windows, you need Visual Studio 2019 16.8 or later (i.e. C17 support. However, earlier versions may work) and the Windows SDK (any recentish one should be fine, but for best results use the latest Windows 11 one {which even works on Windows 7, just as proof of how backwards compatible they are}). On Linux, you need the X11 or Wayland development headers and any version of Clang supporting C2x or later (should be fairly easy to get) or a version of GCC supporting `[[fallthrough]]` in C2x mode (SDL uses it, and GCC 9.3 doesn't support it). You also need CMake. In addition, on Windows and Linux, you have to install the Vulkan SDK (use the installer or your distro's package) as well as [glslc](https://github.com/google/shaderc) and [spirv-cross](https://github.com/KhronosGroup/spirv-cross).

Run CMake like so:
```shell
cmake -S. -Bbuild
cmake --build build
cmake --install build # Installs to `build/install`
```

## Extra CMake options
- `PURPL_ENABLE_DEMO` - Whether to build the demo
- `PURPL_ENABLE_MEMING` - Whether to enable small Easter Eggs in the engine
- `PURPL_USE_SYSTEM_SDL` - Whether to build the engine with the system installation of SDL

## Files included
The following programs are included:
- dos2unix
- ninja
- glslc
- peview (from [Process Hacker](https://github.com/processhacker/processhacker))
They all print licenses or have information about where to find those licenses.

## System-specific notes

On Windows, if you use `clang-cl`, at the time of this writing it doesn't support `/MP`, making it slower and also causing warnings about it. Building with the regular `clang` frontend instead has no such issue, and is faster, so use it unless you have good reason. On macOS, due to limitations in CMake, it is necessary to use XCode for the engine, but it doesn't seem to be working with SDL currently. This makes it necessary to use the system installation of SDL, but at least that shouldn't be too big an issue.

