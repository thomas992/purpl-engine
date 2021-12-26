#!/usr/bin/env python
#  Downloads and builds dependencies in a semi-reusable process
#
#  Copyright 2021 MobSlicer152
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
    if path.__contains__(".git"):
        os.chmod(path, stat.S_IWRITE)
        os.remove(path)
    else:
        print(f"Error processing {path}: {info}")


def sub_cmd(cmd: str, deps: str = "deps") -> str:
    return cmd.replace("<deps>", deps).replace(
        "<move>", "cmd /c move" if os.name == "nt" else "mv"
    )


def download_dep(
    download_cmd: str, setup_cmds: list[str], build_cmd: str, deps: str = "deps"
):
    # Fix the commands
    dl_cmd = sub_cmd(download_cmd, deps)
    stp_cmds = []
    for setup_cmd in setup_cmds:
        stp_cmds.append(sub_cmd(setup_cmd, deps))
    bld_cmd = sub_cmd(build_cmd, deps)

    # Run the commands
    try:
        print(f"Running {dl_cmd}...")
        subprocess.call(dl_cmd)
        for stp_cmd in stp_cmds:
            print(f"Running {stp_cmd}...")
            subprocess.call(stp_cmd)
        print(f"Running {bld_cmd}...")
        subprocess.call(bld_cmd)
    except Exception as e:
        print(f"Failed to run command: {e}")
        exit(1)


# Versions
glew_ver = "2.2.0"

# Dependencies
deps = {
    "cglm": [
        "git clone https://github.com/recp/cglm <deps>/cglm",
        [
            "cmake -S <deps>/cglm -B <deps>/build/cglm -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCGLM_STATIC=OFF"
        ],
        "cmake --build <deps>/build/cglm",
    ],
    "glew": [
        f"curl -fGL https://github.com/nigels-com/glew/releases/download/glew-{glew_ver}/glew-{glew_ver}.tgz -o <deps>/glew.tar.gz",
        [
            "tar xf <deps>/glew.tar.gz -C <deps>",
            f"<move> <deps>/glew-{glew_ver} <deps>/glew",
            "cmake -S <deps>/glew/build/cmake -B <deps>/build/glew -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo",
        ],
        "cmake --build <deps>/build/glew",
    ],
    "sdl2": [
        "git clone https://github.com/libsdl-org/SDL <deps>/sdl2",
        [
            "cmake -S <deps>/sdl2 -B <deps>/build/sdl2 -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSDL_VULKAN=ON -DSDL_STATIC=OFF"
        ],
        "cmake --build <deps>/build/sdl2",
    ],
}

# Folders to get headers from
include_dirs = {
    "cglm": ["deps/tmp/cglm/include"],
    "glew": ["deps/tmp/glew/include"],
    "sdl2": ["deps/tmp/sdl2/include", "deps/tmp/build/sdl2/include"],
}

# Output files that get kept
if os.name == "nt":
    outputs = {
        "cglm": [
            ("deps/tmp/build/cglm/cglm-0.dll", "deps/bin/cglm-0.dll"),
            ("deps/tmp/build/cglm/cglm.lib", "deps/bin/cglm.lib"),
            ("deps/tmp/build/cglm/cglm-0.pdb", "deps/bin/cglm-0.pdb"),
        ],
        "glew": [
            ("deps/tmp/build/glew/bin/glew32.dll", "deps/bin/glew32.dll"),
            ("deps/tmp/build/glew/lib/glew32.lib", "deps/bin/glew.lib"),
            ("deps/tmp/build/glew/bin/glew32.pdb", "deps/bin/glew32.pdb"),
        ],
        "sdl2": [
            ("deps/tmp/build/sdl2/SDL2.dll", "deps/bin/SDL2.dll"),
            ("deps/tmp/build/sdl2/SDL2.lib", "deps/bin/SDL2.lib"),
            ("deps/tmp/build/sdl2/SDL2.pdb", "deps/bin/SDL2.pdb"),
        ],
    }
elif os.name == "posix":
    outputs = {"cglm"}

# Make a folder for the dependencies to be tmpd into
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

# Download the dependencies
print("Downloading dependencies...")
for name, cmds in deps.items():
    print(f"Downloading {name}...")
    download_dep(cmds[0], cmds[1], cmds[2], "deps/tmp")

# Copy the needed outputs and headers
for dep, dirs in include_dirs.items():
    print(f"Copying {dep} headers to deps/include...")
    for dir in dirs:
        for f in os.listdir(dir):
            if os.path.isdir(f"{dir}/{f}"):
                shutil.copytree(f"{dir}/{f}", f"deps/include/{f}")
            else:
                if f.endswith(".h"):
                    shutil.copy(f"{dir}/{f}", f"deps/include/{f}")

for dep, outs in outputs.items():
    for out in outs:
        print(f"Copying {dep} output {out[0]} to {out[1]}...")
        shutil.copy(out[0], out[1])

# Clean up the other files
shutil.rmtree("deps/tmp", onerror=shutil_nuke_git)

print("Done!")
