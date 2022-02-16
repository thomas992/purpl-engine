#!/usr/bin/env python3

#  Downloads and builds dependencies
#  Uses python3 instead of just python because Ubuntu is jank and still thinks
#  Python 2 is recent enough for its executable not to be named differently.
#
#  This script is pretty bad but does work perfectly, and it's simpler than using
#  conan or vcpkg, especially considering I'm using GN for my buildsystem
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
import time


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
	download_cmd: str,
	setup_cmds: list,
	build_cmd: str,
	deps: str = "deps",
	download: bool = True,
):
	# Fix the commands
	dl_cmd = sub_cmd(download_cmd, deps)
	stp_cmds = []
	for setup_cmd in setup_cmds:
		stp_cmds.append(sub_cmd(setup_cmd, deps))
	bld_cmd = sub_cmd(build_cmd, deps)

	# Run the commands (using shell=True is fine because all the commands are in the script)
	try:
		if download or not os.path.exists(dl_cmd.split(" ")[-1]):
			print(f"Running {dl_cmd}...")
			subprocess.run(
				dl_cmd,
				shell=True,
				stdout=sys.stdout,
				stderr=(sys.stdout if dl_cmd.startswith("git") else sys.stderr),
			)
			for stp_cmd in stp_cmds:
				if not os.path.exists(stp_cmd.split(" ")[-1]):
					print(f"Running {stp_cmd}...")
					subprocess.run(
						stp_cmd,
						shell=True,
						stdout=sys.stdout,
						stderr=(sys.stdout if dl_cmd.startswith("git") else sys.stderr),
					)
		print(f"Running {bld_cmd}...")
		subprocess.run(
			bld_cmd,
			shell=True,
			stdout=sys.stdout,
			stderr=(sys.stdout if dl_cmd.startswith("git") else sys.stderr),
		)
	except Exception as e:
		print(f"Failed to run command: {e}")
		exit(1)


# Platform (replacements are for compatibility with GN's OS and architecture naming scheme)
plat = (
	f"{platform.system()}-{platform.machine()}".lower()
#    .replace("x86_64", "x64")
#    .replace("amd64", "x64")
#    .replace("aarch64", "arm64")
#    .replace("windows", "win")
#    .replace("darwin", "mac")
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
no_download = False
for arg in sys.argv:
	if arg in ["debug", "--debug", "--testing"]:
		debug = True
	elif arg in ["dryrun", "--dry-run", "--nothing", "--do-nothing"]:
		dry_run = True
	elif arg in ["keep", "--keep-src", "--keep-tmp"]:
		keep_src = True
	elif arg in ["quiet", "--quiet", "--silent"]:
		silent = True
	elif arg in ["skip-download", "--skip-download", "--no-download"]:
		no_download = True
		keep_src = True
	elif arg in ["verbose", "--verbose", "--loud"]:
		silent = False

# Tools directory
tools_dir = os.path.abspath(sys.argv[0][0 : sys.argv[0].rfind(os.path.sep)])

# Log path
log_path = os.path.join(tools_dir, "deps.log")

# Print a status message
print(f"Building for {plat} with {nproc} jobs")
if debug:
	print("Debug mode")
if dry_run:
	print("Dry run mode")
if keep_src:
	print("Keeping source code")
if no_download:
	print("Not removing existing sources")
if silent:
	print(f"Saving output to {log_path}")
	sys.stdout = open(log_path, "w+", encoding="utf-8")

# CMake is officially the worst build system other than literally just throwing
# together a bunch of random shell scripts and praying to whatever ancient
# Lovecraftian deity is in charge of terrible build systems that it works. Oh
# wait no CMake is still worse, because even this script works better.
cmake_flags = (
	f"-GNinja -DCMAKE_MAKE_PROGRAM={os.getcwd()}/tools/ninja.exe -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl"
	if platform.system() == "Windows"
	else f"-GNinja -DCMAKE_MAKE_PROGRAM={os.getcwd()}/tools/ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++"
)
deps = {
	"cglm": [
		"git clone --depth=1 https://github.com/recp/cglm <deps>/cglm",
		[
			f"cmake -S <deps>/cglm -B <deps>/build/cglm {cmake_flags} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCGLM_STATIC=OFF"
		],
		f"cmake --build <deps>/build/cglm -j{nproc}",
	],
	"phnt": [
		"git clone --depth=1 https://github.com/processhacker/phnt <deps>/phnt"
		if platform.system() == "Windows"
		else "",
		[""],
		"",
	],
	"sdl2": [
		"git clone --depth=1 https://github.com/libsdl-org/SDL <deps>/sdl2",
		[
			f"cmake -S <deps>/sdl2 -B <deps>/build/sdl2 {cmake_flags} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSDL_STATIC=OFF"
		],
		f"cmake --build <deps>/build/sdl2 -j{nproc}",
	],
	"stb": [
		"git clone --depth=1 https://github.com/nothings/stb <deps>/stb",
		[""],
		"",
	],
}

# Folders to get headers from
include_dirs = {
	"cglm": [f"{deps_path}/tmp/cglm/include"],
	"phnt": [f"{deps_path}/tmp/phnt$$"] if platform.system() == "Windows" else [],
	"sdl2": [f"{deps_path}/tmp/sdl2/include", f"{deps_path}/tmp/build/sdl2/include"],
	"stb": [f"{deps_path}/tmp/stb$$"],
}

# Output files that get kept
if platform.system() == "Windows":
	outputs = {
		"cglm": [
			(f"{deps_path}/tmp/build/cglm/cglm-0.dll", f"{deps_path}/bin/cglm-0.dll"),
			(f"{deps_path}/tmp/build/cglm/cglm.lib", f"{deps_path}/bin/cglm.lib"),
			(f"{deps_path}/tmp/build/cglm/cglm-0.pdb", f"{deps_path}/bin/cglm-0.pdb"),
		],
		"sdl2": [
			(f"{deps_path}/tmp/build/sdl2/SDL2.dll", f"{deps_path}/bin/SDL2.dll"),
			(f"{deps_path}/tmp/build/sdl2/SDL2.lib", f"{deps_path}/bin/SDL2.lib"),
			(f"{deps_path}/tmp/build/sdl2/SDL2.pdb", f"{deps_path}/bin/SDL2.pdb"),
		],
	}
elif platform.system() == "Darwin":
	outputs = {
		"cglm": [
			(
				f"{deps_path}/tmp/build/cglm/libcglm.0.8.5.dylib",
				f"{deps_path}/bin/libcglm.dylib",
			),
		],
		"sdl2": [
			(
				f"{deps_path}/tmp/build/sdl2/libSDL2-2.0.dylib",
				f"{deps_path}/bin/libSDL2-2.0.dylib",
			),
		],
	}
elif platform.system() == "Linux":
	outputs = {
		"cglm": [
			(f"{deps_path}/tmp/build/cglm/libcglm.so", f"{deps_path}/bin/libcglm.so"),
			(
				f"{deps_path}/tmp/build/cglm/libcglm.so.0",
				f"{deps_path}/bin/libcglm.so.0",
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

start = time.time_ns()
if not dry_run:
	if not no_download:
		# Prompt to overwrite
		if os.path.exists(deps_path):
			sys.stderr.write(
				f"Dependency folder {deps_path} exists, overwrite it? (y/N) "
			)
			keep = input().lower().split()
			if not len(keep) or keep[0] in ["", "n", "no", "keep"]:
				sys.stderr.write("Not overwriting\n")
				exit()

		shutil.rmtree(deps_path, onerror=shutil_nuke_git)

	if not os.path.exists(deps_path):
		os.mkdir(deps_path)
		os.mkdir(f"{deps_path}/tmp")
		os.mkdir(f"{deps_path}/tmp/build")

	if no_download:
		shutil.rmtree(f"{deps_path}/bin", onerror=shutil_nuke_git)
		shutil.rmtree(f"{deps_path}/include", onerror=shutil_nuke_git)

	os.mkdir(f"{deps_path}/bin")
	os.mkdir(f"{deps_path}/include")

	# Download and build the dependencies
	print("Downloading dependencies...")
	for name, cmds in deps.items():
		print(f"Downloading {name}...")
		download_dep(cmds[0], cmds[1], cmds[2], f"{deps_path}/tmp", not no_download)

	# Copy the needed outputs and headers
	for dep, dirs in include_dirs.items():
		print(f"Copying {dep} headers to {deps_path}/include...")
		for dir in dirs:
			# Check if only one level needs to be copied
			onelevel = False
			if dir.endswith("$$"):
				onelevel = True
			dirname = dir.replace("$$", "")

			if os.path.exists(dirname):
				for f in os.listdir(dirname):
					if os.path.isdir(f"{dirname}/{f}") and not onelevel:
						shutil.copytree(f"{dirname}/{f}", f"{deps_path}/include/{f}")
					else:
						if f.endswith(".h") or f.endswith(".inl"):
							shutil.copy(f"{dirname}/{f}", f"{deps_path}/include/{f}")

	for dep, outs in outputs.items():
		for out in outs:
			if os.path.exists(out[0]) and not os.path.exists(out[1]):
				print(f"Copying {dep} output {out[0]} to {out[1]}...")
				shutil.copy(
					out[0],
					out[1],
					follow_symlinks=(out[0].replace(".dylib", "")[-1] in "1234567890"),
				)
				if platform.system() == "Darwin" and out[0].endswith(".dylib"):
					subprocess.call(
						f"install_name_tool -id {out[1]} {out[1]}",
						shell=True,
						stdout=sys.stdout,
					)

# Clean up the other files
shutil.rmtree(f"{deps_path}/tmp/build", onerror=shutil_nuke_git)
if not keep_src:
	shutil.rmtree(f"{deps_path}/tmp", onerror=shutil_nuke_git)

end = time.time_ns()
buildtime = (end - start) / 1000000
buildminutes = buildtime / 1000 // 60
buildseconds = buildtime / 1000 % 60
buildmillis = buildtime - (int(buildseconds) * 1000)
sys.stderr.write(
	f"Done! Finished building dependencies after {int(buildminutes)}m{int(buildseconds)}s{round(buildmillis, 2)}ms ({buildtime} ms)\n"
)
