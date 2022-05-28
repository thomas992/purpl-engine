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

#include <stdlib.h>
#include <time.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

/// Determine the size of a stack array (for heap arrays, this will be between
/// 1 and the pointer size, which is of no use)
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

/// Convert something to a string
#define PURPL_STRINGIFY(x) PURPL_STRINGIFY_(x)
#define PURPL_STRINGIFY_(x) #x

/// The first of __VA_ARGS__
#define PURPL_FIRST_MACRO_ARG(x, ...) x

/// MSVC defines min/max but other stuff doesn't
#ifdef min
/// Pick the smaller of a and b
#define PURPL_MIN(a, b) min(a, b)
#else // min
/// Pick the smaller of a and b
#define PURPL_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif // min
#ifdef max
/// Pick the larger of a and b
#define PURPL_MAX(a, b) min(a, b)
#else // max
/// Pick the larger of a and b
#define PURPL_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif // max

/// Ignore x
#define PURPL_IGNORE(x) ((void)(x))

/// True if c is a vowel in the Latin alphabet
#define PURPL_ISVOWEL(c) ((c) == 'a' || (c) == 'e' || (c) == 'i' || (c) == 'o' || (c) == 'u')

/// Clamp x to the range [min, max]
#define PURPL_CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#ifdef __ANDROID__
#include <android/log.h>
/// Preinit message
#define PREINIT_MESSAGE(type, ...) __android_log_write(ANDROID_LOG_##type, "purpl-engine", __VA_ARGS__);
#elif defined _WIN32
#include <windows.h>
/// Preinit message
#define PREINIT_MESSAGE(type, ...)                                        \
	do {                                                              \
		fprintf(stderr, PURPL_STRINGIFY_(type) ": " __VA_ARGS__); \
		fprintf(stderr, "\n");                                    \
	} while (0)
#else // __ANDROID__
#include <syslog.h>
#define LOG_ERROR LOG_ERR
/// Preinit message
#define PREINIT_MESSAGE(type, ...)                                        \
	do {                                                              \
		syslog(LOG_##type, __VA_ARGS__);                          \
		fprintf(stderr, PURPL_STRINGIFY_(type) ": " __VA_ARGS__); \
		fprintf(stderr, "\n");                                    \
	} while (0)
#endif // __ANDROID__

#ifdef _WIN32
/// Preinit error
#define PREINIT_ERROR(code, ...)                                                                             \
	do {                                                                                                 \
		MessageBoxA(NULL, PURPL_FIRST_MACRO_ARG(__VA_ARGS__), "Purpl Engine", MB_OK | MB_ICONERROR); \
		PREINIT_MESSAGE(ERROR, __VA_ARGS__);                                                         \
		if (errno != 0)                                                                              \
			PREINIT_MESSAGE(ERROR, "%s (errno %d)", strerror(errno), errno);                     \
		exit(code);                                                                                  \
	} while (0)
#else // _WIN32
/// Preinit error
#define PREINIT_ERROR(code, ...)                                                         \
	do {                                                                             \
		PREINIT_MESSAGE(ERROR, __VA_ARGS__);                                     \
		if (errno != 0)                                                          \
			PREINIT_MESSAGE(ERROR, "%s (errno %d)", strerror(errno), errno); \
		exit(code);                                                              \
	} while (0)
#endif // _WIN32

/// Generate a random number by using RDSEED (x86) or RNDR (ARM), but if that
/// fails then it uses rand() twice. This isn't supposed to be secure at all,
/// but if there's enough reason I'll just make something using OpenSSL.
///
/// \param limit The maximum value
///
/// \return A random number between 0 and limit
extern PURPL_API u64 purpl_random(u64 limit);

/// Get the current process ID
///
/// \return Returns the current process ID
extern PURPL_API u64 purpl_get_pid(void);

/// Trigger a debug breakpoint (debug builds only)
extern PURPL_API void purpl_debug_break(void);
