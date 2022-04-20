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
#include <wchar.h>

#include <functional>

#ifdef _WIN32
#include <windows.h>
#ifdef PURPL_WINRT
#include <winrt/windows.foundation.h>
#include <winrt/windows.storage.h>
#include <winrt/windows.ui.core.h>
#include <winrt/windows.ui.popups.h>
#endif // PURPL_WINRT
#else // _WIN32
#include <dlfcn.h>
#include <errno.h>
#endif // _WIN32

#include "purpl/core/warnings.h"

#define PURPL_IGNORE(x) (void)(x)

typedef int32_t (*purpl_main_t)(int32_t argc, char *argv[]);

#include "exports.h"

#if defined _WIN32 || defined PURPL_WINRT
HMODULE engine_lib = NULL;
#else // PURPL_WINRT
void *engine_lib = NULL;
#endif // PURPL_WINRT

#ifdef _WIN32
#define LIB_EXT ".dll"
#define init_engine_ptrs init_engine_dll_ptrs
#elif __APPLE__
#define LIB_EXT ".dylib"
#define init_engine_ptrs init_engine_dylib_ptrs
#elif __linux__ // _WIN32
#define LIB_EXT ".so"
#define init_engine_ptrs init_engine_so_ptrs
#endif // _WIN32

#ifdef _WIN32
#define purpl_complete_preinit                       \
	((void (*)(purpl_main_t main_func, int argc, \
		   char *argv[]))__imp_purpl_complete_preinit)
#define purpl_internal_shutdown __imp_purpl_internal_shutdown
#else // _WIN32
#define purpl_complete_preinit                       \
	((void (*)(purpl_main_t main_func, int argc, \
		   char *argv[]))purpl_complete_preinit)
#endif // _WIN32

// EXTERN_C is provided by exports.h
EXTERN_C void purpl_preinit(purpl_main_t main_func, int argc, char *argv[])
{
	PURPL_IGNORE(argc);
#ifdef _WIN32
	wchar_t *argv0;
	size_t argv0_len;
	wchar_t *here;
	size_t here_len;
	wchar_t *bin;
	size_t bin_len;
	uint32_t error;

	argv0_len = strlen(argv[0]) + 1;
	argv0 = (wchar_t *)calloc(argv0_len, sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[0], argv0_len, argv0,
			    argv0_len);
	here_len = GetFullPathNameW(argv0, 0, NULL, NULL);
	here = (wchar_t *)calloc(here_len, sizeof(wchar_t));
	GetFullPathNameW(argv0, here_len, here, NULL);
	*wcsrchr(here, L'\\') = 0;

	bin_len = here_len + 5;
	bin = (wchar_t *)calloc(bin_len, sizeof(wchar_t));
	wcsncpy(bin, here, bin_len);
	wcsncat(bin, L"\\bin", bin_len);
	
	fprintf(stderr, "Adding %ls to DLL search paths\n", here);
	if (!AddDllDirectory(here))
		fprintf(stderr,
			"Failed to add %ls to DLL search paths: 0x%" PRIX32 "\n",
			here, GetLastError());
	fprintf(stderr, "Adding %ls to DLL search paths\n", bin);
	if (!AddDllDirectory(bin))
		fprintf(stderr,
			"Failed to add %ls to DLL search paths: 0x%" PRIX32 "\n",
			bin, GetLastError());

	free(bin);
	free(here);

	engine_lib = LoadLibraryExA("engine.dll", NULL,
				    LOAD_LIBRARY_SEARCH_USER_DIRS);
	error = GetLastError();

	if (!engine_lib) {
		fprintf(stderr, "Failed to load engine.dll: 0x%" PRIX32 "\n",
			error);
		MessageBoxA(NULL, "Failed to load engine.dll", "Purpl Engine",
			    MB_OK | MB_ICONERROR);
		exit(STATUS_DLL_NOT_FOUND);
	}

#ifndef PURPL_DEBUG
	// Get rid of the console window in non-debug mode
	FreeConsole();
#endif // !PURPL_DEBUG
#else // _WIN32
      // On POSIX platforms, due to RPATHs being absolute, the engine has to
      // be loaded in a similar way

	char *path;
	size_t path_len = strlen(argv[0]) + strlen("bin/engine" LIB_EXT);

	engine_lib = dlopen("./engine" LIB_EXT, RTLD_NOW);
	if (!engine_lib) {
		// 16 is for the length of "bin/engine.<extension>\0", because
		// that's the longest the path can be
		path = (char *)calloc(path_len + 1, sizeof(char));

		snprintf(path, path_len, "%s", argv[0]);
		strncpy(strrchr(path, '/') + 1, "bin/engine" LIB_EXT,
			path_len + 1);

		fprintf(stderr, "Attempting to load %s\n", path);

		engine_lib = dlopen(path, RTLD_NOW);

		free(path);
	}

	if (!engine_lib) {
		fprintf(stderr, "Failed to load engine" LIB_EXT ", exiting\n");
		exit(ENOENT);
	}
#endif // _WIN32

	init_engine_ptrs(engine_lib);

	fprintf(stderr, "Successfully loaded engine" LIB_EXT "\n");

	// Tell the engine that preinit was called so it doesn't print a
	// warning
	purpl_complete_preinit(main_func, argc, argv);
}

EXTERN_C void purpl_shutdown(void)
{
	purpl_internal_shutdown();
#ifdef _WIN32
	FreeLibrary(engine_lib);
#else // _WIN32
	dlclose(engine_lib);
#endif // _WIN32
}
