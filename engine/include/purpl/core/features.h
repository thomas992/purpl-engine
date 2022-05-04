// Preprocessor maze to detect features like 128-bit integers
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

// Clang and GCC support these and have for like a decade, while MSVC is still
// too lazy to implement them
#if defined __clang__ || defined __GNUC__
// Do a pragma
#define DO_PRAGMA(X) _Pragma(#X)

// Push warnings
#define PURPL_PUSH_WARNINGS() DO_PRAGMA(GCC diagnostic push)

// Pop warnings
#define PURPL_POP_WARNINGS() DO_PRAGMA(GCC diagnostic pop)

// Defined if 128-bit integers are available
#define PURPL_INT128_AVAILABLE 1
#elif defined _MSC_VER
// Do a pragma
#define DO_PRAGMA(X) __pragma(#X)

// Push warnings
#define PURPL_PUSH_WARNINGS() DO_PRAGMA(warning(push))

// Pop warnings
#define PURPL_POP_WARNINGS() DO_PRAGMA(warning(pop))
#endif // __clang__ || __GNUC__

#ifdef PURPL_X86_64
#if defined _MSC_VER
#define PURPL_ASM_CALL
#else // _MSC_VER
#define PURPL_ASM_CALL __attribute__((__ms_abi__))
#endif // _MSC_VER
#endif // PURPL_X86_64
