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

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

void *engine_dll = NULL;

#include "exports.c"

void purpl_preinit(void)
{
	// On Windows, the pre-initialization requires loading the engine DLL, which in turn automatically loads the others
#ifdef _WIN32
	AddDllDirectory(L"bin");
	engine_dll = LoadLibraryA("engine.dll");

	init_ptrs();
#endif // _WIN32
}

