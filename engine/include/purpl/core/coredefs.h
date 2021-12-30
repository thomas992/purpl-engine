// Core definitions for the engine
//
// Copyright 2021 MobSlicer152
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
// Get all the things
#define _ALL_SOURCE
#include <unistd.h>
#endif

#include "types.h"

/// Mark a function as external/imported
#ifdef PURPL_BUILD
#ifdef _MSC_VER
#define PURPL_API __declspec(dllexport)
#else
#define PURPL_API
#endif // _MSC_VER
#else // !PURPL_BUILD
#ifdef _MSC_VER
#define PURPL_API __declspec(dllimport)
#else
#define PURPL_API
#endif // _MSC_VER
#endif // PURPL_BUILD

/// Returns the size of a stack array
#define PURPL_SIZEOF_ARRAY(a) (sizeof((a)) / sizeof((a)[0]))

/// Expands to __PRETTY_FUNCTION__, __FUNCSIG__, or __func__ (unfourtunately,
/// this means it can't always be concatenated with other string literals)
#ifdef __clang__
#define PURPL_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined _MSC_VER
#define PURPL_CURRENT_FUNCTION __FUNCSIG__
#else
#define PURPL_CURRENT_FUNCTION __func__
#endif

/// The current process ID
#ifdef _WIN32
#define PURPL_PROCESS_ID \
	((uint32_t)(size_t)(NtCurrentTeb()->ClientId.UniqueProcess))
#else
#define PURPL_PROCESS_ID getpid()
#endif
