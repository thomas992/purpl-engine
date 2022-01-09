// Miscellaneous utility functions and macros
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

#pragma once

#ifdef _WIN32
// Windows 7 is the earliest version of Windows supported (at least as long as
// it works on my laptop)
#define PHNT_VERSION PHNT_WIN7
#include <phnt.h>
#include <phnt_windows.h>
#else
#include <unistd.h>
#endif

/// Determines the size of a stack array
#define PURPL_SIZEOF_ARRAY(a) (sizeof((a)) / sizeof((a)[0]))

/// Expands to __PRETTY_FUNCTION__, __FUNCSIG__, or __func__ (unfortunately,
/// this means it can't always be concatenated with other string literals)
#ifdef __clang__
#define PURPL_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined _MSC_VER
#define PURPL_CURRENT_FUNCTION __FUNCSIG__
#else
#define PURPL_CURRENT_FUNCTION __func__
#endif

#ifdef _WIN32
/// The current process ID
#define PURPL_PROCESS_ID \
	((uint32_t)(size_t)(NtCurrentTeb()->ClientId.UniqueProcess))

/// The current thread ID
#define PURPL_THREAD_ID \
	((uint32_t)(size_t)(NtCurrentTeb()->ClientId.UniqueThread))
#else
/// The current process ID
#define PURPL_PROCESS_ID getpid()

#ifdef __linux__
extern pid_t gettid(void);

/// The current thread ID
#define PURPL_THREAD_ID gettid()
#endif // __linux__
#endif // _WIN32

/// Generates a random number
#define PURPL_RANDOM(limit) (rand() % (limit))
