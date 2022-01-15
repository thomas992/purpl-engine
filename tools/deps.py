#!/usr/bin/env python
#  Downloads and builds dependencies
#
#  Copyright 2022 MobSlicer152
#  This file is part of Purpl Engine
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

import multiprocessing
import os
import platform
import shutil
import stat
import subprocess


def shutil_nuke_git(e: Exception, path: str, info: Exception):
    # I get that this is good when you're working in a repository,
    # but it makes it annoying to clean them up in scripts like this
    if path.__contains__(".git"):
        os.chmod(path, stat.S_IWRITE)
        os.remove(path)
    else:
        print(f"Error processing {path}: {info}")


def sub_cmd(cmd: str, deps: str = "deps") -> str:
    final = cmd.replace("<deps>", deps).replace(
        "<move>", "move" if os.name == "nt" else "mv"
    )

    # Flip slashes
    if os.name == "nt":
        final = final.replace(deps, deps.replace("/", "\\"))
    else:
        final = final.replace(deps, deps.replace("\\", "/"))

    return final


def download_dep(
    download_cmd: str, setup_cmds: list, build_cmd: str, deps: str = "deps"
):
    # Fix the commands
    dl_cmd = sub_cmd(download_cmd, deps)
    stp_cmds = []
    for setup_cmd in setup_cmds:
        stp_cmds.append(sub_cmd(setup_cmd, deps))
    bld_cmd = sub_cmd(build_cmd, deps)

    # Run the commands (using shell=True is fine because all the commands are in the script)
    try:
        print(f"Running {dl_cmd}...")
        subprocess.run(dl_cmd, shell=True)
        for stp_cmd in stp_cmds:
            print(f"Running {stp_cmd}...")
            subprocess.run(stp_cmd, shell=True)
        print(f"Running {bld_cmd}...")
        subprocess.run(bld_cmd, shell=True)
    except Exception as e:
        print(f"Failed to run command: {e}")
        exit(1)


# Platform (replacements are for compatibility with GN's OS and architecture naming scheme)
plat = f"{platform.system()}-{platform.machine()}".lower().replace("x86_64",
                                                                   "x64").replace("amd64", "x64").replace("aarch64", "arm64").replace("windows", "win").replace("darwin", "mac")

# Versions
glew_ver = "2.2.0"

# Number of processors
nproc = multiprocessing.cpu_count() + 2

# CMake is officially the worst build system other than literally just throwing
# together a bunch of random shell scripts and praying to whatever ancient
# Lovecraftian deity is in charge of terrible build systems that it works
cmake_ninja_bullshit = (
    f"-DCMAKE_MAKE_PROGRAM={os.getcwd()}\\tools\\ninja.exe -DCMAKE_C_COMPILER=cl"
    if os.name == "nt"
    else ""
)

# Dependencies
deps = {
    "bgfx": [
        "git clone https://github.com/bkaradzic/bgfx <deps>/bgfx",
        ["git clone https://github.com/bkaradzic/bx <deps>/bx", "git clone https://github.com/bkaradzic/bimg <deps>/bimg",
            "pushd <deps>\\bgfx && ..\\bx\\tools\\bin\\windows\\genie --platform=x64 --with-sdl --with-tools vs2022 && popd" if os.name == "nt"
            else "pushd <deps>/bgfx && ../bx/tools/bin/linux/genie --gcc=linux-clang --platform=x64 --with-sdl --with-tools gmake && popd"],
        f"msbuild -m:{nproc} -p:Configuration=Release <deps>\\bgfx\\.build\\projects\\vs2022\\bgfx.sln" if os.name == "nt"
        else f"make -C <deps>/bgfx/.build/projects/gmake-linux-clang -j{nproc} config=release"
    ],
    "cglm": [
        "git clone https://github.com/recp/cglm <deps>/cglm",
        [
            f"cmake -S <deps>/cglm -B <deps>/build/cglm {cmake_ninja_bullshit} -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCGLM_STATIC=OFF"
        ],
        f"cmake --build <deps>/build/cglm -j{nproc}",
    ],
    "glew": [
        f"curl -fGL https://github.com/nigels-com/glew/releases/download/glew-{glew_ver}/glew-{glew_ver}.tgz -o <deps>/glew.tar.gz",
        [
            "tar xf <deps>/glew.tar.gz -C <deps>",
            f"<move> <deps>/glew-{glew_ver} <deps>/glew",
            f"cmake -S <deps>/glew/build/cmake -B <deps>/build/glew {cmake_ninja_bullshit} -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo",
        ],
        f"cmake --build <deps>/build/glew -j{nproc}",
    ],
    "phnt": [
        "git clone https://github.com/processhacker/processhacker <deps>/processhacker" if os.name == "nt" else "",
        [""],
        "",
    ],
    "sdl2": [
        "git clone https://github.com/libsdl-org/SDL <deps>/sdl2",
        [
            f"cmake -S <deps>/sdl2 -B <deps>/build/sdl2 {cmake_ninja_bullshit} -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSDL_VULKAN=ON -DSDL_KMSDRM=ON -DSDL_STATIC=OFF"
        ],
        f"cmake --build <deps>/build/sdl2 -j{nproc}",
    ],
    "stb": [
        "git clone https://github.com/nothings/stb <deps>/stb",
        [""],
        "",
    ],
    "vulkan": [
        "curl -fGL https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe -o <deps>/vulkan-sdk.exe",
        ["echo Installing Vulkan SDK"],
        "if not exist C:\\VulkanSDK <deps>\\vulkan-sdk.exe /S",
    ]
    if os.name == "nt"
    else ["", [""], ""],
}

# Folders to get headers from
include_dirs = {
    "bgfx": [f"deps/{plat}/tmp/bgfx/include", f"deps/{plat}/tmp/bx/include", f"deps/{plat}/tmp/bimg/include"],
    "cglm": [f"deps/{plat}/tmp/cglm/include"],
    "glew": [f"deps/{plat}/tmp/glew/include"],
    "phnt": [f"deps/{plat}/tmp/processhacker/phnt/include"] if os.name == "nt" else [],
    "sdl2": [f"deps/{plat}/tmp/sdl2/include", f"deps/{plat}/tmp/build/sdl2/include"],
    "stb": [f"deps/{plat}/tmp/stb$$"],
}

# Output files that get kept
if os.name == "nt":
    outputs = {
        "cglm": [
            (f"deps/{plat}/tmp/build/cglm/cglm-0.dll",
             f"deps/{plat}/bin/cglm-0.dll"),
            (f"deps/{plat}/tmp/build/cglm/cglm.lib",
             f"deps/{plat}/bin/cglm.lib"),
            (f"deps/{plat}/tmp/build/cglm/cglm-0.pdb",
             f"deps/{plat}/bin/cglm-0.pdb"),
        ],
        "glew": [
            (f"deps/{plat}/tmp/build/glew/bin/glewinfo.exe",
             f"deps/{plat}/bin/glewinfo.exe"),
            (f"deps/{plat}/tmp/build/glew/bin/visualinfo.exe",
             f"deps/{plat}/bin/visualinfo.exe"),
            (f"deps/{plat}/tmp/build/glew/bin/glew32.dll",
             f"deps/{plat}/bin/glew32.dll"),
            (f"deps/{plat}/tmp/build/glew/lib/glew32.lib",
             f"deps/{plat}/bin/glew32.lib"),
            (f"deps/{plat}/tmp/build/glew/bin/glew32.pdb",
             f"deps/{plat}/bin/glew32.pdb"),
        ],
        "sdl2": [
            (f"deps/{plat}/tmp/build/sdl2/SDL2.dll",
             f"deps/{plat}/bin/SDL2.dll"),
            (f"deps/{plat}/tmp/build/sdl2/SDL2.lib",
             f"deps/{plat}/bin/SDL2.lib"),
            (f"deps/{plat}/tmp/build/sdl2/SDL2.pdb",
             f"deps/{plat}/bin/SDL2.pdb"),
        ],
    }
elif os.name == "posix":
    outputs = {
        "bgfx": [
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/geometrycRelease",
             f"deps/{plat}/bin/geometryc"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/geometryvRelease",
             f"deps/{plat}/bin/geometryv"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/libbgfxRelease.a",
             f"deps/{plat}/bin/libbgfx.a"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/libbimg_decodeRelease.a",
             f"deps/{plat}/bin/libbimg_decode.a"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/libbimg_encodeRelease.a",
             f"deps/{plat}/bin/libbimg_encode.a"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/libbimgRelease.a",
             f"deps/{plat}/bin/libbimg.a"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/libbxRelease.a",
             f"deps/{plat}/bin/libbx.a"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/shadercRelease",
             f"deps/{plat}/bin/shaderc"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/texturecRelease",
             f"deps/{plat}/bin/texturec"),
            (f"deps/{plat}/tmp/bgfx/.build/linux64_clang/bin/texturevRelease",
             f"deps/{plat}/bin/texturev"),
        ],
        "cglm": [
            (f"deps/{plat}/tmp/build/cglm/libcglm.so",
             f"deps/{plat}/bin/libcglm.so"),
            (f"deps/{plat}/tmp/build/cglm/libcglm.so.0",
             f"deps/{plat}/bin/libcglm.so.0"),
        ],
        "glew": [
            (f"deps/{plat}/tmp/build/glew/bin/glewinfo",
             f"deps/{plat}/bin/glewinfo"),
            (f"deps/{plat}/tmp/build/glew/bin/visualinfo",
             f"deps/{plat}/bin/visualinfo"),
            (f"deps/{plat}/tmp/build/glew/lib/libGLEW.so",
             f"deps/{plat}/bin/libGLEW.so"),
            (f"deps/{plat}/tmp/build/glew/lib/libGLEW.so.2.2",
             f"deps/{plat}/bin/libGLEW.so.2.2"),
        ],
        "sdl2": [
            (f"deps/{plat}/tmp/build/sdl2/libSDL2-2.0.so",
             f"deps/{plat}/bin/libSDL2-2.0.so"),
            (f"deps/{plat}/tmp/build/sdl2/libSDL2-2.0.so.0",
             f"deps/{plat}/bin/libSDL2-2.0.so.0"),
        ],
    }

# Make a folder for the dependencies to be cloned into
shutil.rmtree(f"deps/{plat}", onerror=shutil_nuke_git)

os.mkdir(f"deps/{plat}")
os.mkdir(f"deps/{plat}/tmp")
os.mkdir(f"deps/{plat}/tmp/build")
os.mkdir(f"deps/{plat}/bin")
os.mkdir(f"deps/{plat}/include")

# Download and build the dependencies
print("Downloading dependencies...")
for name, cmds in deps.items():
    print(f"Downloading {name}...")
    download_dep(cmds[0], cmds[1], cmds[2], f"deps/{plat}/tmp")

# Copy the needed outputs and headers
for dep, dirs in include_dirs.items():
    print(f"Copying {dep} headers to deps/{plat}/include...")
    for dir in dirs:
        # Check if only one level needs to be copied
        onelevel = False
        if dir.endswith("$$"):
            onelevel = True
        dirname = dir.replace("$$", "")

        for f in os.listdir(dirname):
            if os.path.isdir(f"{dirname}/{f}") and not onelevel:
                shutil.copytree(f"{dirname}/{f}", f"deps/{plat}/include/{f}")
            else:
                if f.endswith(".h"):
                    shutil.copy(f"{dirname}/{f}", f"deps/{plat}/include/{f}")

for dep, outs in outputs.items():
    for out in outs:
        print(f"Copying {dep} output {out[0]} to {out[1]}...")
        shutil.copy(out[0], out[1], follow_symlinks=(
            out[0][-1] in "1234567890"))

# Clean up the other files
shutil.rmtree(f"deps/{plat}/tmp", onerror=shutil_nuke_git)

print("Done!")
