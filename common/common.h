// Definitions used throughout the engine. Modifying this file is likely to cause a fuckload of stuff to rebuild, so be
// careful.

#pragma once

// Feature macros for POSIX, no harm putting them outside an ifdef
#define _BSD_SOURCE
#define _DEFAULT_SOURCE 1
#define _GNU_SOURCE

#include <assert.h>
#ifdef _WIN32
#include <windows.h>

// This is renamed to make sure no collisions happen
#include "_dirent.h"
#else
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>

#include "cglm/cglm.h"
#include "flecs.h"
#include "SDL.h"
#include "stb_sprintf.h"
#define XXH_INLINE_ALL
#include "xxhash.h"
#include "zstd.h"

#define COMMON_LOG_PREFIX "COMMON: "

// Smaller of two numbers
#define PURPL_MIN(a, b) ((a) < (b) ? (a) : (b))

// Pluralize
#define PURPL_PLURALIZE(count, plural, singular) ((count) == 1 ? (singular) : (plural))

// Make a version
#define PURPL_MAKE_VERSION(major, minor, patch, tweak) (((major) << 24) | ((minor) << 16) | ((patch) << 8) | (tweak))

// Get the major number of a version
#define PURPL_VERSION_MAJOR(version) (((version) >> 24) & 0xFF)

// Get the minor number of a version
#define PURPL_VERSION_MINOR(version) (((version) >> 16) & 0xFF)

// Get the patch number of a version
#define PURPL_VERSION_PATCH(version) (((version) >> 8) & 0xFF)

// Get the tweak number of a version
#define PURPL_VERSION_TWEAK(version) ((version)&0xFF)

// Version number as bitfields
typedef union purpl_version {
	struct {
		uint32_t major:8;
		uint32_t minor:8;
		uint32_t patch:8;
		uint32_t tweak:8;
	};
	uint32_t version;
} purpl_version_t;

// Format string for versions
#define PURPL_PRIVER "%u.%u.%u.%u"

// Macro for putting versions in format functions
#define PURPL_VERSION_FORMAT(version)                                                             \
	PURPL_VERSION_MAJOR(version), PURPL_VERSION_MINOR(version), PURPL_VERSION_PATCH(version), \
		PURPL_VERSION_TWEAK(version)

// Version of the engine
#define PURPL_VERSION PURPL_MAKE_VERSION(1, 0, 0, 0)

// Used to export create_interface
#ifdef _WIN32
#ifdef _MSC_VER
#define PURPL_INTERFACE __declspec(dllexport)
#else
#define PURPL_INTERFACE __attribute__((dllexport))
#endif
#else
#define PURPL_INTERFACE __attribute__((visibility("default")))
#endif

// Byte
typedef uint8_t byte_t;

// Array size
#define PURPL_ARRSIZE(a) (sizeof(a) / sizeof((a)[0]))

#define PURPL_RECAST_FUNCPTR(func, rettype, ...) ((rettype(*)(__VA_ARGS__))(func))

// Instantiate a struct
#define PURPL_STRUCT(s, ...) ((s){ __VA_ARGS__ })

// Function name
#ifdef _MSC_VER
#define PURPL_FUNCNAME __FUNCSIG__
#elif defined __clang__ || defined __GNUC__
#define PURPL_FUNCNAME __PRETTY_FUNCTION__
#else
#define PURPL_FUNCNAME __func__
#endif

// Debug breakpoint
#ifdef _MSC_VER
#define PURPL_DEBUGBREAK() __debugbreak()
#elif defined __clang__
#define PURPL_DEBUGBREAK() __builtin_debugtrap()
#elif defined __GNUC__
#define PURPL_DEBUGBREAK() __builtin_trap()
#else
#define PURPL_DEBUGBREAK()                 \
	{                                  \
		PURPL_LOG("breakpoint\n"); \
		while (1)                  \
			;                  \
	}
#endif

// Make an arbitrary sequence of text a string literal
#define PURPL_STRINGIZE(x) #x

// Make a macro's expansion a string literal
#define PURPL_STRINGIZE_EXPAND(x) PURPL_STRINGIZE(x)

// Percentage
#define PURPL_PERCENT(n, total) (((n) / (double)(total)) * 100)

// Assertion
#define PURPL_ASSERT(cond) assert(cond)
