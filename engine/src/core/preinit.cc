// Pre-initialization functions
//
// Copyright 2022 MobSlicer152
// This file is part of Purpl Engine
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#include <errno.h>
#endif // _WIN32

#include "config.h"

#define PURPL_IGNORE(x) (void)(x)
#define PURPL_SIZEOF_ARRAY(x) (sizeof(x) / sizeof(x[0]))

#ifndef MAX_PATH
#ifdef PATH_MAX
#define MAX_PATH PATH_MAX
#else // PATH_MAX
#define MAX_PATH 260
#endif // PATH_MAX
#endif // MAX_PATH

#ifdef _WIN32
#define PREINIT_ERROR_EX(msg, code, ...)                                                   \
	{                                                                                  \
		MessageBoxA(NULL, msg, "Purpl Engine", MB_OK | MB_ICONERROR);              \
		fprintf(stderr, __VA_ARGS__);                                              \
		if (errno != 0)                                                            \
			fprintf(stderr, "Error: %s (errno %d)\n", strerror(errno), errno); \
		exit(code);                                                                \
	}
#else // _WIN32
#define PREINIT_ERROR_EX(msg, code, ...)                                                   \
	{                                                                                  \
		fprintf(stderr, __VA_ARGS__);                                              \
		if (errno != 0)                                                            \
			fprintf(stderr, "Error: %s (errno %d)\n", strerror(errno), errno); \
		exit(code);                                                                \
	}
#endif // _WIN32
#define PREINIT_ERROR(msg, code) PREINIT_ERROR_EX(msg, code, msg "\n")

typedef int32_t (*purpl_main_t)(int32_t argc, char *argv[]);

#include "exports.h"

#if defined _WIN32 || defined PURPL_WINRT
HMODULE engine_lib = NULL;
#else // PURPL_WINRT
void *engine_lib = NULL;
#endif // PURPL_WINRT

// Yoinked from the Source engine leak (I've seen it in other places, but I was messing with it at the time and was too
// lazy to just Google this) and cleaned up
#ifdef _WIN32
// hinting the nvidia driver to use the dedicated graphics card in an optimus configuration
// for more info, see:
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
EXTERN_C __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// same thing for AMD GPUs using v13.35 or newer drivers
EXTERN_C __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif // _WIN32

#ifdef _WIN32
#define PATH_SEP '\\'
#define PATH_SEP_STR "\\"
#define init_engine_ptrs init_engine_dll_ptrs
#elif __APPLE__
#define LIB_EXT ".dylib"
#define PATH_SEP '/'
#define PATH_SEP_STR "/"
#define init_engine_ptrs init_engine_dylib_ptrs
#else // _WIN32
#define LIB_EXT ".so"
#define PATH_SEP '/'
#define PATH_SEP_STR "/"
#define init_engine_ptrs init_engine_so_ptrs
#endif // _WIN32

#ifdef _WIN32
#define purpl_complete_preinit                                           \
	((int32_t(*)(volatile purpl_main_t main_func, volatile int argc, \
		     volatile char *argv[]))PREFIX(purpl_complete_preinit))
#define purpl_internal_shutdown PREFIX(purpl_internal_shutdown)
#else // _WIN32
#define purpl_complete_preinit \
	((int32_t(*)(volatile purpl_main_t main_func, volatile int argc, volatile char *argv[]))purpl_complete_preinit)
#endif // _WIN32

// EXTERN_C is provided by exports.h
EXTERN_C int32_t purpl_preinit(purpl_main_t main_func, int argc, char *argv[])
{
	PURPL_IGNORE(argc);

	char *here;
	char *path;
	size_t idx;
	FILE *engine_libs_fp;
	char **engine_libs;
	size_t engine_lib_count = 0;
	char *tmp;
	size_t len;
	size_t i;

	here = (char *)calloc(MAX_PATH, sizeof(char));
	if (!here)
		PREINIT_ERROR("Failed to allocate memory for engine path", ENOMEM);

	strncpy(here, argv[0], MAX_PATH);
	*strrchr(here, PATH_SEP) = '\0';

	fprintf(stderr, "Loading Purpl engine\n");

	path = (char *)calloc(MAX_PATH, sizeof(char));
	if (!path)
		PREINIT_ERROR("Failed to allocate memory for engine path", ENOMEM);

	strncpy(path, here, MAX_PATH);
	idx = strlen(here);
	strncpy(path + idx, PATH_SEP_STR "engine_libs.txt\0", MAX_PATH - idx);

	fprintf(stderr, "Searching for engine library list at %s\n", path);
	engine_libs_fp = fopen(path, "rb");
	if (!engine_libs_fp) {
		strncpy(path + idx, PATH_SEP_STR "bin" PATH_SEP_STR "engine_libs.txt\0", MAX_PATH - idx);
		idx += strlen(PATH_SEP_STR "bin");
		fprintf(stderr, "Searching for engine library list at %s\n", path);
		engine_libs_fp = fopen(path, "rb");
	}

	if (!engine_libs_fp)
		PREINIT_ERROR("Failed to open engine library list", ENOENT);

	fprintf(stderr, "Found engine library list at %s\n", path);

	tmp = (char *)calloc(MAX_PATH, sizeof(char));
	if (!tmp)
		PREINIT_ERROR("Failed to allocate memory for engine library list", ENOMEM);
	while (fgets(tmp, MAX_PATH, engine_libs_fp)) {
		if (strncmp(tmp, "//", 2) != 0)
			engine_lib_count++;
	}

	fseek(engine_libs_fp, 0, SEEK_SET);
	i = 0;
	engine_libs = (char **)calloc(engine_lib_count, sizeof(char *));
	if (!engine_libs)
		PREINIT_ERROR("Failed to allocate memory for engine library list", ENOMEM);
	while (fgets(tmp, MAX_PATH, engine_libs_fp)) {
		if (strncmp(tmp, "//", 2) != 0) {
			engine_libs[i] = (char *)calloc(strlen(tmp), sizeof(char));
			if (!engine_libs[i])
				PREINIT_ERROR("Failed to allocate memory for engine library list", ENOMEM);
			strncpy(engine_libs[i], tmp, (strchr(tmp, '\r') ? strchr(tmp, '\r') : strchr(tmp, '\n')) - tmp);
			i++;
		}
	}
	free(tmp);

	for (i = 0; i < engine_lib_count; i++) {
		strncpy(path + idx, PATH_SEP_STR, MAX_PATH - idx);
		len = strlen(engine_libs[i]);
		strncpy(path + idx + 1, engine_libs[i], len);
		fprintf(stderr, "Loading library %s\n", path);
#ifdef _WIN32
		if (!LoadLibraryA(path))
			PREINIT_ERROR("Failed to load library", STATUS_DLL_NOT_FOUND);
#else // _WIN32
		if (!dlopen(path, RTLD_NOW))
			PREINIT_ERROR_EX("Failed to load library", ENOENT, "Failed to load %s: %s\n", path, dlerror());
#endif // _WIN32
		fprintf(stderr, "Loaded library %s\n", path);
	}

#ifdef _WIN32
	engine_lib = GetModuleHandleA("engine.dll");
	static const char *(*wine_get_version)(void);
	*(void **)&wine_get_version = (void *)GetProcAddress(GetModuleHandleA("ntdll.dll"), "wine_get_version");
	if (wine_get_version)
		fprintf(stderr, "Detected that the engine is running in Wine/Proton %s. If you aren't testing this application, see about a"
				" native build of it for your platform\n", wine_get_version());
#else // _WIN32
	engine_lib = dlopen("engine" LIB_EXT, RTLD_NOW);
#endif // _WIN32

	init_engine_ptrs(engine_lib);

	free(path);
	for (i = 0; i < engine_lib_count; i++)
		free(engine_libs[i]);
	free(engine_libs);

	// Calls main
	return purpl_complete_preinit(main_func, argc, (volatile char **)argv);
}

EXTERN_C void purpl_shutdown(void)
{
	purpl_internal_shutdown();
#ifdef _WIN32
	FreeLibrary(engine_lib);
#endif // _WIN32
}
