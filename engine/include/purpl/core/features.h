// Sketchy preprocessor stuff to detect things
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

// Compiler
#ifdef _MSC_VER
#define PURPL_MSVC 1
#elif defined __clang__
#define PURPL_CLANG 1
#elif defined __GNUC__
#define PURPL_GCC 1
#endif // _MSC_VER

// Architecture
#if defined _M_AMD64 || defined __x86_64__
#define PURPL_AMD64 1
#define PURPL_X86 1
#elif defined _M_IX86 || defined __i386__
#define PURPL_IX86 1
#define PURPL_X86 1
#elif defined _M_ARM64 || defined __aarch64__
#define PURPL_ARM 1
#endif // _M_AMD64 || __x86_64__
