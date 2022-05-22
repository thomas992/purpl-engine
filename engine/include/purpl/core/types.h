// Types
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

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __linux__
#include <sys/types.h>
#endif

#include "features.h"

// Boolean
#ifndef __cplusplus
#undef bool
typedef _Bool bool; /// Boolean
#endif // __cplusplus

// Integers
typedef int8_t s8; /// Signed 8-bit integer
typedef int16_t s16; /// Signed 16-bit integer
typedef int32_t s32; /// Signed 32-bit integer
typedef int64_t s64; /// Signed 64-bit integer
#ifdef PURPL_INT128_AVAILABLE
typedef __int128 s128; /// Signed 128-bit integer
#endif // PURPL_INT128_AVAILABLE

// Unsigned integers
typedef uint8_t u8; /// Unsigned 8-bit integer
typedef uint16_t u16; /// Unsigned 16-bit integer
typedef uint32_t u32; /// Unsigned 32-bit integer
typedef uint64_t u64; /// Unsigned 64-bit integer
#ifdef PURPL_INT128_AVAILABLE
typedef unsigned __int128 u128; /// Unsigned 128-bit integer
#endif // PURPL_INT128_AVAILABLE

// Bytes/words
typedef u8 byte; /// Byte
#ifdef PURPL_X86
typedef byte hword; /// Halfword
typedef u16 word; /// Word
typedef u32 dword; /// Doubleword
typedef u64 qword; /// Quadword
#elif defined PURPL_ARM // PURPL_X86
typedef u16 hword; /// Halfword
typedef u32 word; /// Word
typedef u64 dword; /// Doubleword
#ifdef PURPL_INT128_AVAILABLE
typedef u128 qword; /// Quadword
#endif // PURPL_INT128_AVAILABLE
#endif // PURPL_X86
