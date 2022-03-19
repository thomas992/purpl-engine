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
#endif // _WIN32

void purpl_preinit(const char *argv0)
{
	// On Windows, the pre-initialization requires loading the engine DLL,
	// which in turn automatically loads the others
#ifdef _WIN32
	UNICODE_STRING *peb_path =
		&NtCurrentPeb()->ProcessParameters->ImagePathName;
	wchar_t *path;

	engine_dll = LoadLibraryExA("engine.dll", NULL,
				    LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!engine_dll) {
		// 16 is for the length of "bin\\engine.dll\0"
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
		fprintf(stderr, "Failed to load engine.dll, exiting\n");
		exit(1);
	}

	init_ptrs();
#endif // _WIN32
}
