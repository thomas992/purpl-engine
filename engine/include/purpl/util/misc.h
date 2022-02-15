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

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

/// Determines the size of a stack array (for heap arrays, this will be between
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

/// Ignore x
#define PURPL_IGNORE(x) ((void)(x))

/// Generates a random number
#define PURPL_RANDOM(limit) (rand() % (limit))

/// The current process ID
///
/// \return Returns the current process ID
extern PURPL_API u32 purpl_get_pid(void);

/// The current thread ID
///
/// \return Returns the current thread ID
extern PURPL_API u32 purpl_get_tid(void);

/// Trigger a debug breakpoint (debug builds only)
extern PURPL_API void purpl_debug_break(void);
