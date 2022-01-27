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
        "<move>", "move" if platform.system() == "Windows" else "mv"
    )

    # Flip slashes
    if platform.system() == "Windows":
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
        subprocess.run(dl_cmd, shell=True, stdout=sys.stdout)
        for stp_cmd in stp_cmds:
            print(f"Running {stp_cmd}...")
            subprocess.run(stp_cmd, shell=True, stdout=sys.stdout)
        print(f"Running {bld_cmd}...")
        subprocess.run(bld_cmd, shell=True, stdout=sys.stdout)
    except Exception as e:
        print(f"Failed to run command: {e}")
        exit(1)


# Platform (replacements are for compatibility with GN's OS and architecture naming scheme)
plat = (
    f"{platform.system()}-{platform.machine()}".lower()
    .replace("x86_64", "x64")
    .replace("amd64", "x64")
    .replace("aarch64", "arm64")
    .replace("windows", "win")
    .replace("darwin", "mac")
)

# Path to store dependencies in
deps_path = f"deps/{plat}"

# Versions
glew_ver = "2.2.0"

# Number of processors
nproc = multiprocessing.cpu_count() + 2

# Check arguments
debug = False
dry_run = False
keep_src = False
silent = True
for arg in sys.argv:
    if arg in ["debug", "--debug", "--testing"]:
        debug = True
    elif arg in ["dryrun", "--dry-run", "--nothing", "--do-nothing"]:
        dry_run = True
    elif arg in ["keep", "--keep-src", "--keep-tmp"]:
        keep_src = True
    elif arg in ["quiet", "--quiet", "--silent"]:
        silent = True
    elif arg in ["verbose", "--verbose", "--loud"]:
        silent = False

# Log path
log_path = os.path.join(os.path.abspath(sys.argv[0][0 : sys.argv[0].rfind(os.path.sep)]), "deps.log")

# Print a status message
print(f"Building for {plat} with {nproc} jobs")
if debug:
    print("Debug mode")
if dry_run:
    print("Dry run mode")
if keep_src:
    print("Keeping source code")
if silent:
    print(f"Saving output to {log_path}")
    sys.stdout = open(log_path, "w+", encoding="utf-8")

# CMake is officially the worst build system other than literally just throwing
# together a bunch of random shell scripts and praying to whatever ancient
# Lovecraftian deity is in charge of terrible build systems that it works. Oh
# wait no CMake is still worse, because this script works better.
vs_version = "vs2019" if os.getenv("COMPAT") == "1" else "vs2022"
vs_generator = "Visual Studio 17 2022" if vs_version == "vs2022" else "Visual Studio 16 2019"
cmake_flags = (
    f"-DCMAKE_C_COMPILER=cl -G\"{vs_generator}\""
    if platform.system() == "Windows"
    else "-GNinja"
)
bgfx_config = "Debug" if keep_src else "Release"
deps = {
    "bgfx": [
        "git clone https://github.com/bkaradzic/bgfx <deps>/bgfx",
        [
            "git clone https://github.com/bkaradzic/bx <deps>/bx",
            "git clone https://github.com/bkaradzic/bimg <deps>/bimg",
            f"pushd <deps>\\bgfx && ..\\bx\\tools\\bin\\windows\\genie --platform=x64 --with-shared-lib --with-tools {vs_version} && popd"
            if platform.system() == "Windows"
            else f"cd <deps>/bgfx && ../bx/tools/bin/linux/genie --gcc=linux-clang --platform=x64 --with-shared-lib --with-tools gmake && cd {os.getcwd()}"
            if platform.system() == "Linux"
            else "",
        ],
        f"msbuild -m:{nproc} -p:Configuration={bgfx_config} <deps>\\bgfx\\.build\\projects\\{vs_version}\\bgfx.sln"
        if platform.system() == "Windows"
        else f"make -C <deps>/bgfx/.build/projects/gmake-linux-clang -j{nproc} config={bgfx_config.lower()}64 bgfx-shared-lib shaderc texturec texturev geometryc geometryv"
        if platform.system() == "Linux"
        else "",
    ],
    "cglm": [
        "git clone https://github.com/recp/cglm <deps>/cglm",
        [
            f"cmake -S <deps>/cglm -B <deps>/build/cglm {cmake_flags} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCGLM_STATIC=OFF"
        ],
        f"cmake --build <deps>/build/cglm -j{nproc}",
    ],
    "glew": [
        f"curl -fGL https://github.com/nigels-com/glew/releases/download/glew-{glew_ver}/glew-{glew_ver}.tgz -o <deps>/glew.tar.gz",
        [
            "tar xf <deps>/glew.tar.gz -C <deps>",
            f"<move> <deps>/glew-{glew_ver} <deps>/glew",
            f"cmake -S <deps>/glew/build/cmake -B <deps>/build/glew {cmake_flags} -DCMAKE_BUILD_TYPE=RelWithDebInfo",
        ],
        f"cmake --build <deps>/build/glew -j{nproc}",
    ],
    "phnt": [
        "git clone https://github.com/processhacker/phnt <deps>/phnt"
        if platform.system() == "Windows"
        else "",
        [""],
        "",
    ],
    "sdl2": [
        "git clone https://github.com/libsdl-org/SDL <deps>/sdl2",
        [
            f"cmake -S <deps>/sdl2 -B <deps>/build/sdl2 {cmake_flags} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSDL_STATIC=OFF"
        ],
        f"cmake --build <deps>/build/sdl2 -j{nproc}",
    ],
    "stb": [
        "git clone https://github.com/nothings/stb <deps>/stb",
        [""],
        "",
    ],
}

# Folders to get headers from
include_dirs = {
    "bgfx": [
        f"{deps_path}/tmp/bgfx/include",
        f"{deps_path}/tmp/bx/include",
        f"{deps_path}/tmp/bimg/include",
    ],
    "cglm": [f"{deps_path}/tmp/cglm/include"],
    "glew": [f"{deps_path}/tmp/glew/include"],
    "phnt": [f"{deps_path}/tmp/phnt$$"]
    if platform.system() == "Windows"
    else []
    if platform.system() == "Linux"
    else [],
    "sdl2": [f"{deps_path}/tmp/sdl2/include", f"{deps_path}/tmp/build/sdl2/include"],
    "stb": [f"{deps_path}/tmp/stb$$"],
}

# Output files that get kept
if platform.system() == "Windows":
    outputs = {
        "bgfx": [
            (
                f"{deps_path}/tmp/bgfx/.build/win64_{vs_version}/bin/geometryc{bgfx_config}.exe",
                f"{deps_path}/bin/geometryc.exe",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/win64_{vs_version}/bin/geometryv{bgfx_config}.exe",
                f"{deps_path}/bin/geometryv.exe",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/win64_{vs_version}/bin/bgfx-shared-lib{bgfx_config}.dll",
                f"{deps_path}/bin/bgfx.dll",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/win64_{vs_version}/bin/bgfx-shared-lib{bgfx_config}.pdb",
                f"{deps_path}/bin/bgfx.pdb",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/win64_{vs_version}/bin/shaderc{bgfx_config}.exe",
                f"{deps_path}/bin/shaderc.exe",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/win64_{vs_version}/bin/texturec{bgfx_config}.exe",
                f"{deps_path}/bin/texturec.exe",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/win64_{vs_version}/bin/texturev{bgfx_config}.exe",
                f"{deps_path}/bin/texturev.exe",
            ),
        ],
        "cglm": [
            (f"{deps_path}/tmp/build/cglm/cglm-0.dll", f"{deps_path}/bin/cglm-0.dll"),
            (f"{deps_path}/tmp/build/cglm/cglm.lib", f"{deps_path}/bin/cglm.lib"),
            (f"{deps_path}/tmp/build/cglm/cglm-0.pdb", f"{deps_path}/bin/cglm-0.pdb"),
        ],
        "glew": [
            (
                f"{deps_path}/tmp/build/glew/bin/glewinfo.exe",
                f"{deps_path}/bin/glewinfo.exe",
            ),
            (
                f"{deps_path}/tmp/build/glew/bin/visualinfo.exe",
                f"{deps_path}/bin/visualinfo.exe",
            ),
            (
                f"{deps_path}/tmp/build/glew/bin/glew32.dll",
                f"{deps_path}/bin/glew32.dll",
            ),
            (
                f"{deps_path}/tmp/build/glew/lib/glew32.lib",
                f"{deps_path}/bin/glew32.lib",
            ),
            (
                f"{deps_path}/tmp/build/glew/bin/glew32.pdb",
                f"{deps_path}/bin/glew32.pdb",
            ),
        ],
        "sdl2": [
            (f"{deps_path}/tmp/build/sdl2/SDL2.dll", f"{deps_path}/bin/SDL2.dll"),
            (f"{deps_path}/tmp/build/sdl2/SDL2.lib", f"{deps_path}/bin/SDL2.lib"),
            (f"{deps_path}/tmp/build/sdl2/SDL2.pdb", f"{deps_path}/bin/SDL2.pdb"),
        ],
    }
elif platform.system() == "Linux":
    outputs = {
        "bgfx": [
            (
                f"{deps_path}/tmp/bgfx/.build/linux64_clang/bin/geometryc{bgfx_config}",
                f"{deps_path}/bin/geometryc",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/linux64_clang/bin/geometryv{bgfx_config}",
                f"{deps_path}/bin/geometryv",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/linux64_clang/bin/libbgfx-shared-lib{bgfx_config}.so",
                f"{deps_path}/bin/libbgfx.so",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/linux64_clang/bin/shaderc{bgfx_config}",
                f"{deps_path}/bin/shaderc",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/linux64_clang/bin/texturec{bgfx_config}",
                f"{deps_path}/bin/texturec",
            ),
            (
                f"{deps_path}/tmp/bgfx/.build/linux64_clang/bin/texturev{bgfx_config}",
                f"{deps_path}/bin/texturev",
            ),
        ],
        "cglm": [
            (f"{deps_path}/tmp/build/cglm/libcglm.so", f"{deps_path}/bin/libcglm.so"),
            (
                f"{deps_path}/tmp/build/cglm/libcglm.so.0",
                f"{deps_path}/bin/libcglm.so.0",
            ),
        ],
        "glew": [
            (f"{deps_path}/tmp/build/glew/bin/glewinfo", f"{deps_path}/bin/glewinfo"),
            (
                f"{deps_path}/tmp/build/glew/bin/visualinfo",
                f"{deps_path}/bin/visualinfo",
            ),
                (
                f"{deps_path}/tmp/build/glew/lib/libGLEW.so",
                f"{deps_path}/bin/libGLEW.so",
            ),
            (
                f"{deps_path}/tmp/build/glew/lib/libGLEW.so.2.2",
                f"{deps_path}/bin/libGLEW.so.2.2",
            ),
        ],
        "sdl2": [
            (
                f"{deps_path}/tmp/build/sdl2/libSDL2-2.0.so",
                f"{deps_path}/bin/libSDL2-2.0.so",
            ),
            (
                f"{deps_path}/tmp/build/sdl2/libSDL2-2.0.so.0",
                f"{deps_path}/bin/libSDL2-2.0.so.0",
            ),
        ],
    }

if not dry_run:
    # Prompt to overwrite
    if os.path.exists(deps_path):
        sys.stderr.write(f"Dependency folder {deps_path} exists, overwrite it (y/N) ")
        keep = input().lower().split()
        if not len(keep) or keep[0] in ["", "n", "no", "keep"]:
            sys.stderr.write("Not overwriting\n")
            exit()
    
    # Make a folder for the dependencies to be cloned into
    shutil.rmtree(deps_path, onerror=shutil_nuke_git)
    os.mkdir(deps_path)
    os.mkdir(f"{deps_path}/tmp")
    os.mkdir(f"{deps_path}/tmp/build")
    os.mkdir(f"{deps_path}/bin")
    os.mkdir(f"{deps_path}/include")

    # Download and build the dependencies
    print("Downloading dependencies...")
    for name, cmds in deps.items():
        print(f"Downloading {name}...")
        download_dep(cmds[0], cmds[1], cmds[2], f"{deps_path}/tmp")

    # Copy the needed outputs and headers
    for dep, dirs in include_dirs.items():
        print(f"Copying {dep} headers to {deps_path}/include...")
        for dir in dirs:
            # Check if only one level needs to be copied
            onelevel = False
            if dir.endswith("$$"):
                onelevel = True
            dirname = dir.replace("$$", "")

            for f in os.listdir(dirname):
                if os.path.isdir(f"{dirname}/{f}") and not onelevel:
                    shutil.copytree(f"{dirname}/{f}", f"{deps_path}/include/{f}")
                else:
                    if f.endswith(".h") or f.endswith(".inl"):
                        shutil.copy(f"{dirname}/{f}", f"{deps_path}/include/{f}")

    for dep, outs in outputs.items():
        for out in outs:
            print(f"Copying {dep} output {out[0]} to {out[1]}...")
            shutil.copy(out[0], out[1], follow_symlinks=(out[0][-1] in "1234567890"))

if not dry_run or (dry_run and debug):
    if platform.system() == "Windows":
        print(f"Generating new bgfx import library for {plat[4:]}")
        bgfx_def = outputs["bgfx"][2][1].replace("dll", "def")
        subprocess.call(f"mkdlldef.bat " + outputs["bgfx"][2][1], shell=True, stdout=sys.stdout)
        subprocess.call(f"link /lib /nologo /out:{deps_path}/bin/bgfx.lib /machine:{plat[4:]} /def:{bgfx_def}", shell=True, stdout=sys.stdout)

# Clean up the other files
if not keep_src:
    shutil.rmtree(f"{deps_path}/tmp", onerror=shutil_nuke_git)

print("Done!")
