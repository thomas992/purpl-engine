// Structures containing data for the engine instance related to graphics APIs
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

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

// Vulkan supports everything except macOS (MoltenVK exists,
// but I'd rather just write it in Metal eventually)
// Vulkan information
#ifndef __APPLE__
#include <vulkan/vulkan.h>

/// Vulkan queue family information
struct vulkan_queue_families {
	size_t graphics_family; // The index of the graphics queue
				// family
	bool graphics_family_present; // Whether graphics_family is set
};

struct purpl_instance_vulkan {
	VkInstance inst; // Vulkan instance
	VkDebugUtilsMessengerEXT debug_messenger; // Debug messenger

#ifdef GLAD_VULKAN_H_
	GLADapiproc (*vk_get_instance_proc_addr)(VkInstance inst,
						 const char *name);
#else // GLAD_VULKAN_H_
	void *(*vk_get_instance_proc_addr)(VkInstance inst, const char *name);
#endif // GLAD_VULKAN_H_

	VkPhysicalDevice phys_device; // Physical device
	struct vulkan_queue_families phys_device_queue_families; // The queue
								 // families to
								 // use with
								 // phys_device

	VkDevice device; // Logical device
};
#endif // !__APPLE__
