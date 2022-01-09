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

import os
import shutil
import stat
import subprocess
import sys

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


# Versions
glew_ver = "2.2.0"

# CMake is officially the worst build system other than literally just throwing
# together a bunch of random shell scripts and praying to whatever ancient
# Lovecraftian deity is in charge of terrible build systems that it works
cmake_ninja_bullshit = f"-DCMAKE_MAKE_PROGRAM={os.getcwd()}\\tools\\ninja.exe" if os.name == "nt" else ""

# Dependencies
deps = {
    "cglm": [
        "git clone https://github.com/recp/cglm <deps>/cglm",
        [
            f"cmake -S <deps>/cglm -B <deps>/build/cglm {cmake_ninja_bullshit} -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCGLM_STATIC=OFF"
        ],
        "cmake --build <deps>/build/cglm",
    ],
    "glew": [
        f"curl -fGL https://github.com/nigels-com/glew/releases/download/glew-{glew_ver}/glew-{glew_ver}.tgz -o <deps>/glew.tar.gz",
        [
            "tar xf <deps>/glew.tar.gz -C <deps>",
            f"<move> <deps>/glew-{glew_ver} <deps>/glew",
            f"cmake -S <deps>/glew/build/cmake -B <deps>/build/glew {cmake_ninja_bullshit} -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo",
        ],
        "cmake --build <deps>/build/glew",
    ],
    "phnt": [
        "git clone https://github.com/processhacker/processhacker <deps>/processhacker",
        [""],
        "",
    ],
    "sdl2": [
        "git clone https://github.com/libsdl-org/SDL <deps>/sdl2",
        [
            f"cmake -S <deps>/sdl2 -B <deps>/build/sdl2 {cmake_ninja_bullshit} -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSDL_VULKAN=ON -DSDL_KMSDRM=ON -DSDL_STATIC=OFF"
        ],
        "cmake --build <deps>/build/sdl2",
    ],
    "stb": [
        "git clone https://github.com/nothings/stb <deps>/stb",
        [""],
        "",
    ],
    "vulkan": [
        "curl -fGL https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe -o <deps>/vulkan-sdk.exe",
        ["git clone https://github.com/KhronosGroup/Vulkan-Headers <deps>/vulkan-headers"],
        "<deps>/vulkan-sdk.exe"
    ] if os.name == "nt" else [
        f"git clone https://github.com/KhronosGroup/Vulkan-Headers <deps>/vulkan-headers",
        [""],
        f""
    ]
}

# Folders to get headers from
include_dirs = {
    "cglm": ["deps/tmp/cglm/include"],
    "glew": ["deps/tmp/glew/include"],
    "phnt": ["deps/tmp/processhacker/phnt/include"],
    "sdl2": ["deps/tmp/sdl2/include", "deps/tmp/build/sdl2/include"],
    "stb": ["deps/tmp/stb$$"],
    "vulkan": ["deps/tmp/vulkan-headers/include"]
}

# Output files that get kept
if os.name == "nt":
    outputs = {
        "cglm": [
            ("deps/tmp/build/cglm/libcglm-0.dll", "deps/bin/libcglm-0.dll"),
            ("deps/tmp/build/cglm/libcglm-0.lib", "deps/bin/libcglm-0.lib"),
            ("deps/tmp/build/cglm/libcglm-0.pdb", "deps/bin/libcglm-0.pdb"),
        ],
        "glew": [
            ("deps/tmp/build/glew/bin/glewinfo.exe", "deps/bin/glewinfo.exe"),
            ("deps/tmp/build/glew/bin/visualinfo.exe", "deps/bin/visualinfo.exe"),
            ("deps/tmp/build/glew/bin/glew32.dll", "deps/bin/glew32.dll"),
            ("deps/tmp/build/glew/lib/glew32.lib", "deps/bin/glew.lib"),
            ("deps/tmp/build/glew/bin/glew32.pdb", "deps/bin/glew32.pdb"),
        ],
        "sdl2": [
            ("deps/tmp/build/sdl2/SDL2.dll", "deps/bin/SDL2.dll"),
            ("deps/tmp/build/sdl2/SDL2.lib", "deps/bin/SDL2.lib"),
            ("deps/tmp/build/sdl2/SDL2.pdb", "deps/bin/SDL2.pdb"),
        ],
        "vulkan": [
            ("deps/tmp/vulkan-sdk.exe", "deps/bin/vulkan-sdk.exe")
        ]
    }
elif os.name == "posix":
    outputs = {
        "cglm": [
            ("deps/tmp/build/cglm/libcglm.so", "deps/bin/libcglm.so"),
            ("deps/tmp/build/cglm/libcglm.so.0", "deps/bin/libcglm.so.0"),
        ],
        "glew": [
            ("deps/tmp/build/glew/bin/glewinfo", "deps/bin/glewinfo"),
            ("deps/tmp/build/glew/bin/visualinfo", "deps/bin/visualinfo"),
            ("deps/tmp/build/glew/lib/libGLEW.so", "deps/bin/libGLEW.so"),
            ("deps/tmp/build/glew/lib/libGLEW.so.2.2", "deps/bin/libGLEW.so.2.2"),
        ],
        "sdl2": [
            ("deps/tmp/build/sdl2/libSDL2-2.0.so", "deps/bin/libSDL2-2.0.so"),
            ("deps/tmp/build/sdl2/libSDL2-2.0.so.0", "deps/bin/libSDL2-2.0.so.0"),
        ]
    }

# Make a folder for the dependencies to be cloned into
shutil.rmtree("deps/bin", onerror=shutil_nuke_git)
shutil.rmtree("deps/tmp", onerror=shutil_nuke_git)
shutil.rmtree("deps/include", onerror=shutil_nuke_git)
try:
    os.mkdir("deps/tmp")
    os.mkdir("deps/tmp/build")
    os.mkdir("deps/bin")
    os.mkdir("deps/include")
except Exception as e:
    pass

# Download and build the dependencies
print("Downloading dependencies...")
for name, cmds in deps.items():
    print(f"Downloading {name}...")
    download_dep(cmds[0], cmds[1], cmds[2], "deps/tmp")

# Copy the needed outputs and headers
for dep, dirs in include_dirs.items():
    print(f"Copying {dep} headers to deps/include...")
    for dir in dirs:
        # Check if only one level needs to be copied
        onelevel = False
        if dir.endswith("$$"):
            onelevel = True
        dirname = dir.replace("$$", "")

        for f in os.listdir(dirname):
            if os.path.isdir(f"{dirname}/{f}") and not onelevel:
                shutil.copytree(f"{dirname}/{f}", f"deps/include/{f}")
            else:
                if f.endswith(".h"):
                    shutil.copy(f"{dirname}/{f}", f"deps/include/{f}")

for dep, outs in outputs.items():
    for out in outs:
        print(f"Copying {dep} output {out[0]} to {out[1]}...")
        shutil.copy(out[0], out[1], follow_symlinks=(out[0][-1] in "1234567890"))

# Clean up the other files
shutil.rmtree("deps/tmp", onerror=shutil_nuke_git)

print("Done!")
