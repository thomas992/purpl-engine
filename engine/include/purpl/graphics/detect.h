// GPU identification
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

#ifdef _MSC_VER
#else // _MSC_VER
#include <cpuid.h>
#endif // _MSC_VER

#ifdef PURPL_BUILD
#include <bgfx/c99/bgfx.h>
#endif // PURPL_BUILD

#include "purpl/core/coredefs.h"
#include "purpl/core/features.h"
#include "purpl/core/types.h"

/// Detect if an old iGPU is present
///
/// \param info A pointer to a bgfx_platform_data_t struct initialized by
/// 		bgfx_set_platform_data
///
/// \return Returns true if an iGPU not supporting Vulkan is being used
#ifdef PURPL_BUILD
extern PURPL_API bool purpl_check_igpu(bgfx_caps_t *info);
#else // PURPL_BUILD
extern PURPL_API bool purpl_check_igpu(void *info);
#endif // PURPL_BUILD
