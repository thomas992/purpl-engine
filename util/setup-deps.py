#!/usr/bin/env python
#  Sketchy script to download dependencies
#
#  Copyright 2021 MobSlicer152
#  This file is part of Shard C Library 2
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

import glob
import io
import os
import shutil
import sys
import urllib.request
from zipfile import ZipFile

# Folder to put dependencies in
DEPS_FOLDER = os.path.abspath(sys.argv[0]).replace(os.path.join("util", f"{__name__}.py"), "deps")

# Submodules
SUBMODULES = ["glm", "sdl", "spdlog", "stb"]

# Versions of things that snapshots are downloaded for
GLEW_VER = "2.2.0"

# URLs to get snapshots from
GLEW_URL = f"https://github.com/nigels-com/glew/releases/download/glew-{GLEW_VER}/glew-{GLEW_VER}.zip"

# Download a file
def get_file(url: str):
    # Get the file
    try:
        resp = urllib.request.urlopen(url)
    except:
        print("Failed to download {url}")

    return resp.read()

# Unzip a file
def unzip(data: bytes, outdir: str):
    zip = ZipFile(io.BytesIO(data))
    zip.extractall(outdir)

# Remove a version name from a folder
def remove_version(base_name: str, version: str, sep: str = "-"):
    if os.path.exists(base_name):
        shutil.rmtree(base_name)
    shutil.move(f"{base_name}{sep}{version}", base_name)

# Build a CMake project
def cmake_build(dir: os.PathLike, args: list, defs: list):
    args_str = ""
    defs_str = ""

    # Put arguments in a string
    generator_set = False
    for _arg in args:
        args_str = f"{args_str}{' ' if args_str.len() > 0 else ''}{_arg}"
        if _arg.find("-G"):
            generator_set = True

    # Set the generator to Ninja as a fallback
    if not generator_set:
        args_str = f"{args_str} -GNinja"

    # Put definitions in a string
    for _def in defs:
        defs_str = f"{defs_str}{' ' if defs_str.len() > 0 else ''}{'-D' if _def.len() > 0 and _def[1] != 'D' else ''}{_def}"

    # Change directory and run CMake
    prev_dir = os.getcwd()
    print(f"Entering directory {dir}")
    os.chdir(dir)
    cmake = os.popen(f"cmake -S. -Bbuild {args_str} {defs_str}")
    cmake = os.close(cmake)
    if cmake != None:
        print(f"CMake exited with code {cmake}, exiting")
        sys.exit(cmake)

# Clone submodules
print("Cloning submodules...")
os.popen("git submodule update --init")
for _module in SUBMODULES:
    print(f"Updating submodule {_module}...")
    os.chdir(os.path.join(DEPS_FOLDER, _module))
    os.popen("git pull origin")
print("Done updating submodules")

# Download other dependencies
print(f"Using {DEPS_FOLDER} to store non-submodule dependencies")

print(f"Downloading GLEW {GLEW_VER} from {GLEW_URL} and extracting to {DEPS_FOLDER}")
glew_zip = get_file(GLEW_URL)
unzip(glew_zip, DEPS_FOLDER)
remove_version(os.path.join(os.path.join(DEPS_FOLDER, "glew")), GLEW_VER)
glew_zip = bytes("", "utf-8")

print("Done downloading snapshots")
