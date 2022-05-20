// Synchronization things
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

#include "stb_ds.h"

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/log.h"
#include "purpl/core/types.h"

#include "purpl/util/util.h"

/// Create all needed synchronization objects
///
/// \return Returns true if creation was successful, false otherwise
extern bool vulkan_create_sync_objs(void);

/// Destroy synchronization objects
extern void vulkan_destroy_sync_objs(void);

/// Create presentation and render semaphores
///
/// \return Returns true if creation was successful, false otherwise
extern bool vulkan_create_semaphores(void);

/// Create the render fence
///
/// \return Returns true if creation was successful, false otherwise
extern bool vulkan_create_render_fence(void);

