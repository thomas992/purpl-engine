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
#include <vulkan/vulkan_core.h>

bool vulkan_create_logical_device(void)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;
	VkPhysicalDeviceFeatures phys_device_features = { 0 }; // This will be
							       // used later
	VkDeviceCreateInfo device_create_info = { 0 };
	float queue_priority = 1.0f;
	VkDeviceQueueCreateInfo *queue_create_infos = NULL;
	size_t queue_create_info_count = 1;
	VkResult result;

	PURPL_LOG_INFO(purpl_inst->logger, "Creating logical device");

	stbds_arrsetlen(queue_create_infos, queue_create_info_count);

	memset(&queue_create_infos[0], 0, sizeof(VkDeviceQueueCreateInfo));
	queue_create_infos[0].sType =
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_infos[0].queueFamilyIndex =
		vulkan->phys_device_queue_families.graphics_family;
	queue_create_infos[0].queueCount = 1;
	queue_create_infos[0].pQueuePriorities = &queue_priority;
	if (vulkan->phys_device_queue_families.graphics_family !=
	    vulkan->phys_device_queue_families.presentation_family) {
		PURPL_LOG_INFO(
			purpl_inst->logger,
			"Graphics queue family is different from presentation queue family, performance might be slightly worse");
		queue_create_info_count = 2;
		stbds_arrsetlen(queue_create_infos, queue_create_info_count);
		memset(&queue_create_infos[1], 0,
		       sizeof(VkDeviceQueueCreateInfo));

		queue_create_infos[1].sType =
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_infos[1].queueFamilyIndex =
			vulkan->phys_device_queue_families.presentation_family;
		queue_create_infos[1].queueCount = 1;
		queue_create_infos[1].pQueuePriorities = &queue_priority;
	}

	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pEnabledFeatures = &phys_device_features;
	device_create_info.enabledExtensionCount = 0;
	device_create_info.queueCreateInfoCount = queue_create_info_count;
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
