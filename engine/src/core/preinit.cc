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
#endif // _WIN32

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
#define purpl_complete_preinit ((void (*)(purpl_main_t main_func, int argc, char *argv[]))__imp_purpl_complete_preinit)
#define purpl_internal_shutdown __imp_purpl_internal_shutdown
#else // _WIN32
#define purpl_complete_preinit ((void (*)(purpl_main_t main_func, int argc, char *argv[]))purpl_complete_preinit)
#endif // _WIN32

// EXTERN_C is provided by exports.h
EXTERN_C void purpl_preinit(purpl_main_t main_func, int argc, char *argv[])
{
	PURPL_IGNORE(argc);

	// On Windows, the pre-initialization requires loading the engine DLL,
	// which in turn causes NTDLL to automatically load the others
#ifdef _WIN32
	char *path;
	char *abs_path;
	size_t abs_path_len;
	size_t size = strlen(argv[0]) + 16;

	engine_lib = LoadLibraryExA("engine.dll", NULL,
				    LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!engine_lib) {
		// 16 is for the length of "bin\\engine.dll\0", because that's
		// the longest the path can be
		path = (char *)calloc(size, sizeof(char));
		if (!path) {
			fprintf(stderr,
				"Failed to allocate memory for preinit, exiting to avoid crash\n");
			exit(1);
		}
		
		strncpy(path, argv[0], strlen(argv[0]));
		strncpy(strrchr(path, '\\') + 1, "bin\\engine.dll", 16);
		
		abs_path_len = GetFullPathNameA(path, 0, NULL, NULL);
		abs_path = (char *)calloc(abs_path_len, sizeof(char));
		if (!abs_path) {
			fprintf(stderr,
				"Failed to allocate memory for preinit, exiting to avoid crash\n");
			exit(1);
		}
		GetFullPathNameA(path, abs_path_len, abs_path, NULL);

		engine_lib = LoadLibraryExA(
			abs_path, NULL,
			LOAD_LIBRARY_SEARCH_DEFAULT_DIRS |
				LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

		free(abs_path);
		free(path);
	}

	if (!engine_lib) {
#ifdef PURPL_WINRT
		winrt::Windows::UI::Popups::MessageDialog box(
			winrt::hstring(L"Purpl Engine"),
			winrt::hstring(L"Failed to load engine.dll"));
		box.ShowAsync();
#else // PURPL_WINRT
		MessageBoxA(NULL, "Failed to load engine.dll", "Purpl Engine",
			    MB_OK | MB_ICONERROR);
#endif // PURPL_WINRT
		fprintf(stderr, "Failed to load engine.dll, exiting\n");
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

	engine_lib = dlopen("./engine" LIB_EXT, RTLD_NOW);
	if (!engine_lib) {
		// 16 is for the length of "bin/engine.<extension>\0", because
		// that's the longest the path can be
		path = (char *)calloc(strlen(argv[0]) + strlen("bin/engine" LIB_EXT) + 1, sizeof(char));
		if (!path) {
			fprintf(stderr,
				"Failed to allocate memory for preinit, exiting to avoid crash\n");
			exit(1);
		}

		strncpy(path, argv[0], strlen(argv[0]));
		strncpy(strrchr(path, '/') + 1, "bin/engine" LIB_EXT,
			strlen("bin/engine" LIB_EXT) + 1);

		engine_lib = dlopen(path, RTLD_NOW);

		free(path);
	}

	if (!engine_lib) {
		fprintf(stderr, "Failed to load engine" LIB_EXT ", exiting\n");
		exit(1);
	}
#endif // _WIN32

	init_engine_ptrs(engine_lib);
#ifdef PURPL_DEBUG
	printf("Successfully loaded engine" LIB_EXT "\n");
#endif // PURPL_DEBUG

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
