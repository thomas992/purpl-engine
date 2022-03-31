// Functions for creating a logical device
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

#include "purpl/graphics/vulkan/logical_device.h"

bool vulkan_create_logical_device(void)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;
	VkPhysicalDeviceFeatures phys_device_features = { 0 }; // This will be
							       // used later
	VkDeviceCreateInfo device_create_info = { 0 };
	float queue_priority = 1.0f; // Only need one queue
	VkDeviceQueueCreateInfo *queue_create_infos = NULL;
	size_t unique_queue_families[] = {
		vulkan->phys_device_queue_families.graphics_family,
		vulkan->phys_device_queue_families.presentation_family
	};
	size_t i;
	VkResult result;

	PURPL_LOG_INFO(purpl_inst->logger, "Creating logical device");

	stbds_arrsetcap(queue_create_infos, PURPL_SIZEOF_ARRAY(unique_queue_families));

	for (i = 0; i < PURPL_SIZEOF_ARRAY(unique_queue_families); i++) {
		VkDeviceQueueCreateInfo queue_create_info = { 0 };

		PURPL_LOG_INFO(
			purpl_inst->logger,
			"Filling creation information for queue family %zu (index %zu)",
			i + 1, unique_queue_families[i]);

		queue_create_info.sType =
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = unique_queue_families[i];
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;

		stbds_arrput(queue_create_infos, queue_create_info);
	}

	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pEnabledFeatures = &phys_device_features;
	device_create_info.enabledExtensionCount = 0;
	device_create_info.queueCreateInfoCount =
		PURPL_SIZEOF_ARRAY(unique_queue_families);
	device_create_info.pQueueCreateInfos = queue_create_infos;

	result = vkCreateDevice(vulkan->phys_device, &device_create_info, NULL,
				&vulkan->device);
	if (result != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to create logical device: VkResult %d",
				result);
		return false;
	}

	stbds_arrfree(queue_create_infos);

	PURPL_LOG_INFO(purpl_inst->logger,
		       "Successfully created logical device with handle 0x%X",
		       vulkan->device);

	vkGetDeviceQueue(
		vulkan->device,
		vulkan->phys_device_queue_families.presentation_family, 0,
		&vulkan->presentation_queue);
	PURPL_LOG_INFO(purpl_inst->logger,
		       "Retrieved handle 0x%X for presentation queue",
		       vulkan->presentation_queue);

	return true;
}
