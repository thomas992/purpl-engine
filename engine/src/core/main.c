// Entry point
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

#ifdef _WIN32
#include <stddef.h>

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

// THREADNAME_INFO struct, copied from SDL2
#pragma pack(push, 8)
typedef struct _THREADNAME_INFO {
	DWORD dwType;
	LPCSTR szName;
	DWORD dwThreadID;
	DWORD dwFlags;
} THREADNAME_INFO;
#pragma pack(pop)
#else // _WIN32
#define _GNU_SOURCE
#include <pthread.h>

extern int32_t pthread_setname_np(pthread_t thread, const char *name);
#endif // _WIN32

#include "purpl/core/init.h"

#include "purpl/util/misc.h"

#undef main

int32_t main(int32_t argc, char *argv[])
{
	int32_t ret = 0;

	PREINIT_MESSAGE(INFO, "Setting main thread name to \"main\"");
#ifdef _WIN32
	HRESULT (*SetThreadDescription_l)(HANDLE, wchar_t *);
	THREADNAME_INFO thr_info = { 0 };

	SetThreadDescription_l = GetProcAddress(GetModuleHandle("kernel32.dll", "SetThreadDescription");
	if (SetThreadDescription_l)
		SetThreadDescription_l(GetCurrentThread(), L"main");

	// Also do weird shit using SEH to set the thread name for older Windows versions (only works if a debugger is attached)
	if (IsDebuggerPresent()) {
		thr_info.dwType = 0x1000;
		strncpy(thr_info.szName, "main");
		thr_info.dwThreadID = -1;

		RaiseException(0x406D1388, 0, sizeof(thr_info) / sizeof(ULONG), (const ULONG_PTR *)&thr_info);
	}
#else // _WIN32
	pthread_setname_np(pthread_self(), "main");
#endif // _WIN32

	ret = purpl_preinit((void *[]){purpl_main, (void *)(int64_t)argc, argv});
	purpl_shutdown();

	exit(ret);
}
