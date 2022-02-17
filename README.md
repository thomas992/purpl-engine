# Purpl Engine
[![CI](https://github.com/MobSlicer152/purpl-engine/actions/workflows/main.yml/badge.svg)](https://github.com/MobSlicer152/purpl-engine/actions/workflows/main.yml)

This is my fifth game engine, it's gonna be good this time

## Building the engine

All the stuff needed to build the engine (other than the compiler and `bfg9000`) is provided, and licenses for the libraries and programs are in `deps/licenses`.

On Windows, you need Visual Studio 2019 16.8 or later (i.e. C17 support. However, earlier versions may work) and the Windows SDK (any recentish one should be fine, but for best results use the latest Windows 11 one {which even works on Windows 7, just as proof of how backwards compatible they are}). On Linux, you need the X11 or Wayland development headers and any version of Clang supporting C17 or later (basically every easily obtainable version unless you run a super old distro). You also need to install `bfg9000` with `pip` to generate the build files (`bfg9000[msbuild]` on Windows).

To build the engine, run the following commands on Windows:
```batch
:: For other architectures than x64, look at the buildenv script and run the commands but tweak them appropriately
tools\buildenv

:: Easiest way to get Meson
pip install meson

meson --prefix %CD%\build 
ninja -C out\win
```
On Linux, run these commands instead:
```sh
# Somehow install Meson, it's better to get it from your package manager but pip has it too
pip install meson

# GCC is also supported but not as good
CC=clang LD=ld.lld AR=llvm-ar meson out/linux --prefix $(pwd)/build
ninja -C out/linux
```
On macOS:
```sh
pip install meson

meson out/linux --prefix $(pwd)/build

ninja -C out/mac
```

To see more build options, run `meson --help`

To run the demo, do the following on Windows:
```batch
:: buildenv already sets the path so that the DLLs are found for the engine and dependencies
demo
```
On Linux, run this command:
```sh
# Running a locally built binary, the rpaths are fine
out/linux/demo/demo
```
On macOS run this:
```sh
# Running a locally built binary, the rpaths are fine
out/mac/demo/demo
```

## Using the releases and CI artifacts
You can download artifacts from the "Actions" section for a build of the latest commit of the engine. To use these builds, do the following if you're on Linux or macOS:
```sh
# DYLD_LIBRARY_PATH for macOS, LD_LIBRARY_PATH for Linux
[DY]LD_LIBRARY_PATH=bin ./demo
```
On Windows, you can run the `purpl-demo.bat` file.
