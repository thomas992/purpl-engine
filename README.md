# Purpl Engine
[![CI](https://github.com/MobSlicer152/purpl-engine/actions/workflows/main.yml/badge.svg)](https://github.com/MobSlicer152/purpl-engine/actions/workflows/main.yml)

This is my fifth game engine, it's gonna be good this time

## Building the engine

All the stuff needed to build the engine (other than the compiler) is provided, and licenses for the libraries and programs are in `deps/licenses`.

On Windows, you need Visual Studio 2019 16.8 or later (i.e. C17 support. However, earlier versions may work) and the Windows SDK (any recentish one should be fine). On Linux, you need the X11 or Wayland development headers and any version of Clang supporting C17 or later (basically every easily obtainable version unless you run a super old distro).

To build the engine, run the following commands on Windows:
```batch
tools\buildenv
python tools\deps.py

:: This is more intended to be run from Explorer, it generates a compilation database and a Visual Studio solution too
tools\mkprojects

:: This is the better way for a Command Prompt shell
gn gen out
ninja -C out
```
On Linux, run these commands instead:
```sh
tools/deps.py # After running this script, you may want to install the Vulkan SDK (for convenience, the latest source tarball is downloaded into deps/bin)
tools/gn gen out
ninja -C out
```
GN can also generate IDE projects with `--ide=vs` or `--ide=xcode` (XCode and macOS are not tested because I deleted my Hackintosh install {though I am planning to buy an M1 Mac}, but they might work with some modifications to the GN scripts and the `tools/deps.py` script), and compilation databases with `--export-compile-commands` for language servers in (Neo)vim and Visual Studio Code. To set options for the build such as debugging info, meming (various small Easter Eggs such as April 1st being reported as March 32nd and years being relative to 1970 on April 1st only), and verbose logging.

To run the demo, do the following on Windows:
```batch
out\main :: buildenv already sets the path so that the DLLs are found for the engine and dependencies
```
On Linux, run this command:
```sh
LD_LIBRARY_PATH=deps/bin:out out/main
```

## Using the releases and CI artifacts
You can download artifacts from the actions sections for a build of the latest commit of the engine. To use these builds, do the following if you're on Linux:
```sh
chmod +x purpl-demo main

# This script sets the environment variable needed to run the demo
./purpl-demo

# Or, if you don't want to use the script
LD_LIBRARY_PATH=. ./main
```
