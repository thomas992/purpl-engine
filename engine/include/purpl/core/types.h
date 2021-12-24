// Types
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

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#ifdef PURPL_BUILD
#define PURPL_PREFIX(x)
#else // PURPL_BUILD
#define PURPL_PREFIX(x) purpl_##x
#endif // PURPL_BUILD

// Integers
typedef int8_t PURPL_PREFIX(s8);
typedef int16_t PURPL_PREFIX(s16);
typedef int32_t PURPL_PREFIX(s32);
typedef int64_t PURPL_PREFIX(s64);
#ifdef __clang__
typedef __int128_t PURPL_PREFIX(s128);
#endif // __clang__

// Unsigned integers
typedef uint8_t PURPL_PREFIX(u8);
typedef uint16_t PURPL_PREFIX(u16);
typedef uint32_t PURPL_PREFIX(u32);
typedef uint64_t PURPL_PREFIX(u64);
#ifdef  __clang__
typedef __uint128_t PURPL_PREFIX(u128);
#endif // __clang__

// Bytes/words
typedef PURPL_PREFIX(u8) PURPL_PREFIX(byte);
#ifdef PURPL_X86
typedef PURPL_PREFIX(byte) PURPL_PREFIX(hword);
typedef PURPL_PREFIX(u16) PURPL_PREFIX(word);
typedef PURPL_PREFIX(u32) PURPL_PREFIX(dword);
typedef PURPL_PREFIX(u64) PURPL_PREFIX(qword);
#elif defined PURPL_ARM // PURPL_X86
typedef PURPL_PREFIX(u16) PURPL_PREFIX(hword);
typedef PURPL_PREFIX(u32) PURPL_PREFIX(word);
typedef PURPL_PREFIX(u64) PURPL_PREFIX(dword);
#ifdef __clang__
typedef u128 PURPL_PREFIX(qword);
#endif // __clang__
#endif // PURPL_X86
