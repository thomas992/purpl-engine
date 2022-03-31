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
#include <wchar.h>

#ifdef _WIN32
#define PHNT_VERSION PHNT_THRESHOLD
#include <phnt_windows.h>
#include <phnt.h>
#endif // _WIN32

void *engine_dll = NULL;

#ifdef _WIN32
#include "exports.h"

#define purpl_complete_preinit __imp_purpl_complete_preinit
#else // _WIN32
extern void purpl_complete_preinit(void);
#endif // _WIN32

void purpl_preinit(void)
{
	// On Windows, the pre-initialization requires loading the engine DLL,
	// which in turn causes NTDLL to automatically load the others
#ifdef _WIN32
	UNICODE_STRING *peb_path =
		&NtCurrentPeb()->ProcessParameters->ImagePathName;
	wchar_t *path;

	engine_dll = LoadLibraryExA("engine.dll", NULL,
				    LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!engine_dll) {
		// 16 is for the length of "bin\\engine.dll\0", because that's the worst case
		// scenario for the needed length of the final string
		path = calloc(peb_path->Length + 16, sizeof(wchar_t));
		if (!path) {
			fprintf(stderr,
				"Failed to allocate memory for preinit, exiting to avoid crash\n");
			exit(1);
		}

		wcsncpy(path, peb_path->Buffer, peb_path->Length);
		wcsncpy(wcsrchr(path, L'\\') + 1, L"bin\\engine.dll", 16);

		engine_dll = LoadLibraryExW(
			path, NULL,
			LOAD_LIBRARY_SEARCH_DEFAULT_DIRS |
				LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

		free(path);
	}

	if (!engine_dll) {
		MessageBoxA(NULL, "Failed to load engine.dll", "Purpl Engine",
			    MB_OK | MB_ICONERROR);
		fprintf(stderr, "Failed to load engine.dll, exiting\n");
		exit(STATUS_DLL_NOT_FOUND);
	}

#ifndef PURPL_DEBUG
	// Get rid of the console window in non-debug mode
	FreeConsole();
#endif // !PURPL_DEBUG

	init_engine_dll_ptrs();
#endif // _WIN32

	// Tell the engine that preinit was called so it doesn't print a
	// warning
	purpl_complete_preinit();
}
