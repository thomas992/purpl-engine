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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef _WIN32
#define PHNT_VERSION PHNT_THRESHOLD
#include <phnt_windows.h>
#include <phnt.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include "purpl/core/types.h"
#define PURPL_IGNORE(x) (void)(x)

#include "exports.h"

void *engine_lib = NULL;

#ifdef _WIN32
#define purpl_complete_preinit __imp_purpl_complete_preinit
#endif // _WIN32

void purpl_preinit(int argc, char *argv[])
{
	// On Windows, the pre-initialization requires loading the engine DLL,
	// which in turn causes NTDLL to automatically load the others
#ifdef _WIN32
	PURPL_IGNORE(argc);
	PURPL_IGNORE(argv);

	UNICODE_STRING *peb_path =
		&NtCurrentPeb()->ProcessParameters->ImagePathName;
	wchar_t *path;

	engine_lib = LoadLibraryExA("engine.dll", NULL,
				    LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!engine_lib) {
		// 16 is for the length of "bin\\engine.dll\0", because that's
		// the longest the path can be
		path = calloc(peb_path->Length + 16, sizeof(wchar_t));
		if (!path) {
			fprintf(stderr,
				"Failed to allocate memory for preinit, exiting to avoid crash\n");
			exit(1);
		}

		wcsncpy(path, peb_path->Buffer, peb_path->Length);
		wcsncpy(wcsrchr(path, L'\\') + 1, L"bin\\engine.dll", 16);

		engine_lib = LoadLibraryExW(
			path, NULL,
			LOAD_LIBRARY_SEARCH_DEFAULT_DIRS |
				LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

		free(path);
	}

	if (!engine_lib) {
		MessageBoxA(NULL, "Failed to load engine.dll", "Purpl Engine",
			    MB_OK | MB_ICONERROR);
		fprintf(stderr, "Failed to load engine.dll, exiting\n");
		exit(STATUS_DLL_NOT_FOUND);
	}

#ifndef PURPL_DEBUG
	// Get rid of the console window in non-debug mode
	FreeConsole();
#endif // !PURPL_DEBUG

	init_engine_dll_ptrs(engine_lib);
#else // _WIN32
      // On POSIX platforms, due to RPATHs being absolute, the engine has to
      // be loaded in a similar way

	char *path;

	PURPL_IGNORE(argc);

	engine_lib = dlopen("./libengine.so", RTLD_NOW);
	if (!engine_lib) {
		// 16 is for the length of "bin/libengine.so\0", because that's
		// the longest the path can be
		path = calloc(strlen(argv[0]) + 16, sizeof(char));
		if (!path) {
			fprintf(stderr,
				"Failed to allocate memory for preinit, exiting to avoid crash\n");
			exit(1);
		}

		strncpy(path, argv[0], strlen(argv[0]));
		strncpy(strrchr(path, '/') + 1, "bin/libengine.so", 16);

		engine_lib = dlopen(path, RTLD_NOW);

		free(path);
	}

	if (!engine_lib) {
		fprintf(stderr, "Failed to load libengine.so, exiting\n");
		exit(1);
	}

	init_libengine_so_ptrs(engine_lib);
#endif // _WIN32

	// Tell the engine that preinit was called so it doesn't print a
	// warning
	purpl_complete_preinit();
}
