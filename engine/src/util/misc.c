// Miscellaneous utility functions
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

#if defined _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <processthreadsapi.h>
#elif defined __APPLE__ // _WIN32
#define _ALL_SOURCE
#include <pthread.h>
#include <unistd.h>
#else // _WIN32
#ifdef __linux__
#define _GNU_SOURCE
#endif // __linux__
#define _ALL_SOURCE
#include <unistd.h>
#endif

#include "purpl/util/misc.h"

#include "purpl/core/warnings.h"

extern u64 PURPL_ASM_CALL purpl_arch_random(void);

PURPL_API u64 purpl_random(u64 limit)
{
	// Entropy
	srand(purpl_arch_random() * time(NULL) | purpl_get_tid());
	return (purpl_arch_random() ^ rand() + purpl_get_pid()) % (limit + 1);
}

PURPL_API u64 purpl_get_pid(void)
{
#ifdef _WIN32
	return GetCurrentProcessId();
#else
	return getpid();
#endif // _WIN32
}

PURPL_API u64 purpl_get_tid(void)
{
#if defined _WIN32
	return GetCurrentThreadId();
#elif defined __APPLE__
	u64 id;

	pthread_threadid_np(NULL, &id);
	return id;
#elif defined __linux__
	return gettid();
#elif defined __FreeBSD__
	return 0;
#endif // _WIN32
}

PURPL_API void purpl_debug_break(void)
{
#ifdef PURPL_DEBUG
#ifdef _MSC_VER
	__debugbreak();
#elif __clang__ // _MSC_VER
	__builtin_debugtrap();
#elif __GNUC__ // _MSC_VER
	__builtin_trap();
#else // _MSC_VER
#endif // _MSC_VER
#endif // PURPL_DEBUG
}
