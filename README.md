My 6th attempt at creating a game engine. This time around some inspiration is taken from Quake 2 and Source engine, because they're cool engines.

## Build instructions
Install CMake, any development libraries for X11, Wayland, etc, the Vulkan SDK, and a compiler that supports C11 or above (I like Clang/LLVM and MSVC better than GNU C, so those are more likely to work). Also make sure to clone the submodules. After all that, just run CMake normally (there are toolchain files for the Microsoft Game Development Kit that can supposedly compile for Xbox if you have the GDKX, but I don't so no clue if those work).

