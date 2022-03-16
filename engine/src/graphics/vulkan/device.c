// Physical device functions
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

#include "purpl/graphics/vulkan/device.h"

u64 vulkan_score_device(VkPhysicalDevice device)
{
	u64 score = 0;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;

	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);

	// iGPUs are usually shit, and on a system that doesn't follow that
	// rule there probably won't be a dGPU
	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	score += properties.limits.maxImageDimension2D;

	return score;
}

bool vulkan_pick_physical_device(void)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;
	VkPhysicalDevice *devices = NULL;
	VkPhysicalDeviceProperties properties;
	u32 device_count = 0;
	u64 best_score = 0;
	size_t best_idx = 0;
	size_t i;

	PURPL_LOG_INFO(purpl_inst->logger, "Enumerating render devices");

	vkEnumeratePhysicalDevices(vulkan->inst, &device_count, NULL);
	if (!device_count) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to locate a device with Vulkan support");
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Found %u devices", device_count);

	stbds_arrsetlen(devices, device_count);
	vkEnumeratePhysicalDevices(vulkan->inst, &device_count, devices);

	for (i = 0; i < stbds_arrlenu(devices); i++) {
		vkGetPhysicalDeviceProperties(devices[i], &properties);
		u64 score = vulkan_score_device(devices[i]);
		PURPL_LOG_INFO(purpl_inst->logger, "Device %zu:", i + 1);
		PURPL_LOG_INFO(purpl_inst->logger, "\tName: %s",
			       properties.deviceName);
		PURPL_LOG_INFO(purpl_inst->logger, "\tScore: %d", score);

		if (score > best_score) {
			vulkan->phys_device = devices[i];
			best_score = score;
			best_idx = i;
		}
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Device %zu (%s) has the best score",
		       best_idx + 1, properties.deviceName);

	stbds_arrfree(devices);
	return true;
}
