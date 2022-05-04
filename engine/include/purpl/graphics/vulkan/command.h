// Vulkan command stuff
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

#include <vulkan/vulkan.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/log.h"
#include "purpl/core/types.h"

#include "purpl/util/util.h"

/// Create a command pool
///
/// \return Returns true if successful, false otherwise
extern bool vulkan_create_command_pool(void);

/// Allocate a command buffer
///
/// \return Returns true if successful, false otherwise
extern bool vulkan_alloc_command_buffer(void);
