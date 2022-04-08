// Swapchain related functions
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

#include "purpl/graphics/vulkan/swapchain.h"

bool vulkan_get_swapchain_info(VkPhysicalDevice device, struct vulkan_swapchain_info *info)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	u32 format_count = 0;
	u32 present_mode_count = 0;

	if (!info) {
		PURPL_LOG_DEBUG(purpl_inst->logger, "info is NULL, returning");
		return false;
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkan->surface, &info->capabilities);
	
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->surface, &format_count, NULL);
	if (!format_count) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to get surface formats");
		return false;
	}
	stbds_arrsetlen(info->formats, format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->surface, &format_count, info->formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->surface, &present_mode_count, NULL);
	if (!present_mode_count) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to get surface present modes");
		return false;
	}
	stbds_arrsetlen(info->present_modes, present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->surface, &present_mode_count, info->present_modes);

	return true;
}

