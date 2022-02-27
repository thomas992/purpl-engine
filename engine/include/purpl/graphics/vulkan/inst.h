// Functions for creating Vulkan instances
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

#ifndef PURPL_BUILD
#error This header should only be included in engine files
#endif // !PURPL_BUILD

#include <vulkan/vulkan.h>

#include "SDL.h"
#include "SDL_vulkan.h"

#include "stb_ds.h"

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/types.h"

#include "purpl/util/util.h"

/// Create a Vulkan instance
///
/// \return Returns true on success, false on failure
extern bool vulkan_create_instance(void);

/// Get the list of extensions to request
///
/// \return Returns an array of two pointers, one to an array of strings, and
///	    one to the list of supported instance extensions. All three
///	    pointers are to be freed by stbds_arrfree.
extern void **vulkan_get_extensions(void);

