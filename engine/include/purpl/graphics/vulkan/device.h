// Physical device funtions
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

#include "vulkan/vulkan_core.h"
#ifndef PURPL_BUILD
#error This header should only be includeed in engine files
#endif // !PURPL_BUILD

#include <vulkan/vulkan.h>

#include "SDL.h"
#include "stb_ds.h"

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/types.h"

#include "purpl/util/util.h"

/// Score a device based on its properties
///
/// \param device The device to score
///
/// \return Returns the device's score
extern u64 vulkan_score_device(VkPhysicalDevice device, size_t idx);

/// Pick the best physical device
///
/// \return Returns the device chosen
extern bool vulkan_pick_physical_device(void);

/// Get the queue families to be used for a device
///
/// \param device The device to get queue families for
///
/// \return Returns true if the device has all the necessary queue families
extern bool vulkan_get_device_queue_families(VkPhysicalDevice device, struct vulkan_queue_families *queue_families);

