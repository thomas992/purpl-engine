// Utility functions for Vulkan
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

#include <string.h>

#include <stb_sprintf.h>
#include <vulkan/vulkan.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

/// Convert a VkResult to a string
///
/// \param result The result code to convert
///
/// \return Returns a static buffer of PURPL_STATIC_BUF_MAX bytes containing
/// the translated error code
extern const char *purpl_vulkan_strerror(VkResult result);
