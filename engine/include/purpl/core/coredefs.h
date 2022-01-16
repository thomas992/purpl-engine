// Core definitions for the engine
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

#include "types.h"

/// Mark a function as external/imported on Windows
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

/// Make a version number from major, minor, and patch numbers
#define PURPL_MAKE_VERSION(major, minor, patch) \
	(((major)&0xFF) << 16 | ((minor)&0xFF) << 8 | (patch)&0xFF)

/// The current major version of the engine
#define PURPL_MAJOR_VERSION 1

/// The current minor version of the engine
#define PURPL_MINOR_VERSION 0

/// The current patch version of the engine
#define PURPL_PATCH_VERSION 0

/// The full version of the engine
#define PURPL_VERSION                                                \
	PURPL_MAKE_VERSION(PURPL_MAJOR_VERSION, PURPL_MINOR_VERSION, \
			   PURPL_PATCH_VERSION)

/// The size of the static buffers returned by some string functions
#define PURPL_STATIC_BUF_MAX 128
