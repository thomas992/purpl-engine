// Definitions used throughout the engine

#pragma once

#ifdef _WIN32
// This is renamed to make sure no collisions happen
#include "_dirent.h"
#else
#include <dirent.h>
#endif
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL.h"
#undef main // No SDL_main

#include "stb_ds.h"
#include "stb_sprintf.h"

#define XXH_INLINE_ALL
#include "xxhash.h"
#ifdef PURPL_X64
#include "xxh_x86dispatch.h"
#endif

#include "zstd.h"

// Make a version
#define PURPL_MAKE_VERSION(major, minor, patch, tweak) (((major) << 24) | ((minor) << 16) | ((patch) << 8) | (tweak))

// Get the major number of a version
#define PURPL_VERSION_MAJOR(version) (((version) >> 24) & 0xFF)

// Get the minor number of a version
#define PURPL_VERSION_MINOR(version) (((version) >> 16) & 0xFF)

// Get the patch number of a version
#define PURPL_VERSION_PATCH(version) (((version) >> 8) & 0xFF)

// Get the tweak number of a version
#define PURPL_VERSION_TWEAK(version) ((version) & 0xFF)

// Version of the engine
#define PURPL_VERSION PURPL_MAKE_VERSION(1, 0, 0, 0)

// Used to export create_interface
#ifdef _MSC_VER
#define PURPL_INTERFACE __declspec(dllexport)
#else
#define PURPL_INTERFACE __attribute__((dllexport))
#endif

// Byte
typedef uint8_t byte_t;

// Array size
#define PURPL_ARRSIZE(a) (sizeof(a) / sizeof((a)[0]))

// Function name
#ifdef _MSC_VER
#define PURPL_FUNCNAME __FUNCSIG__
#elif defined __clang__ || defined __GNUC__
#define PURPL_FUNCNAME __PRETTY_FUNCTION__
#else
#define PURPL_FUNCNAME __func__
#endif

// Print a message to the console
#define PURPL_LOG(...) printf(__VA_ARGS__);

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
#define PURPL_ASSERT(cond)                                                                                    \
	if (!(cond)) {                                                                                        \
		fprintf(stderr, "Assertion PURPL_ASSERT(" #cond ") at %s:%d:%s failed\n", __FILE__, __LINE__, \
			PURPL_FUNCNAME);                                                                      \
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Assertion failed",                            \
					 "Assertion \"" #cond "\" failed at " __FILE__                        \
					 ":" PURPL_STRINGIZE_EXPAND(__LINE__),                                \
					 NULL);                                                               \
		PURPL_DEBUGBREAK();                                                                           \
	}
