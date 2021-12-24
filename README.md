# Purpl Engine
This is my fifth game engine, it's gonna be good this time

## Building the engine

All the stuff needed to build the engine is provided, and licenses for the libraries and programs are in `deps/licenses`.

To build the engine, run the following commands on Windows:
```batch
tools\buildenv
python tools\deps.py

rem This is more intended to be run from Explorer, it generates a compilation database and a Visual Studio solution too
tools\mkprojects

rem This is the better way for a Command Prompt shell
gn gen out
ninja -C out
```
On Linux, run these commands instead:
```sh
tools/deps.py
tools/gn gen out
ninja -C out
```
GN can also generate IDE projects with `--ide=vs` or `--ide=xcode` (XCode and macOS are not tested because I deleted my Hackintosh install, but they might work with some modifications to the GN scripts), and compilation databases with `--export-compile-commands`.
