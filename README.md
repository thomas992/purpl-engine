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
python tools\deps.py

:: This is more intended to be run from Explorer, it generates a compilation database and a Visual Studio solution too
tools\mkprojects

:: This is the better way for a Command Prompt (or PowerShell if you aren't a true Windows god like me) shell
bfg9000 configure out\win
ninja -C out\win
```
On Linux, run these commands instead:
```sh
tools/deps.py # You can pass --skip-download to prevent redownloading, and --keep-src to keep sources for debugging
bfg9000 configure out/linux
ninja -C out/linux
```
On macOS:
```sh
tools/deps.py
bfg9000 configure out/mac
ninja -C out/mac
```

To see more build options, run `bfg9000 --help` or `bfg9000 <action> --help`

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
You can download artifacts from the actions sections for a build of the latest commit of the engine. To use these builds, do the following if you're on Linux or macOS:
```sh
chmod +x purpl-demo

# This script sets the environment variable needed to run the demo
./purpl-demo

# Or, if you don't want to use the script (DYLD_LIBRARY_PATH for macOS, LD_LIBRARY_PATH for Linux)
[DY]LD_LIBRARY_PATH=. ./main
```
