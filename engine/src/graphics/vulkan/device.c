// Device selection and creation functions
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

bool vulkan_pick_physical_device(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkPhysicalDevice *devices = NULL;
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

	PURPL_LOG_INFO(purpl_inst->logger, "Found %u device%s", device_count,
		       device_count == 1 ? "" : "s");

	stbds_arrsetlen(devices, device_count);
	vkEnumeratePhysicalDevices(vulkan->inst, &device_count, devices);

	for (i = 0; i < stbds_arrlenu(devices); i++) {
		u64 score = vulkan_score_device(devices[i], i);

		if (score > best_score) {
			vulkan->phys_device = devices[i];
			best_score = score;
			best_idx = i;
		}
	}

	if (!vulkan->phys_device || best_score == 0) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to locate a device with Vulkan support");
		return false;
	}

	vkGetPhysicalDeviceFeatures(vulkan->phys_device,
				    &vulkan->phys_device_features);
	vkGetPhysicalDeviceProperties(vulkan->phys_device,
				      &vulkan->phys_device_properties);
	PURPL_LOG_INFO(purpl_inst->logger,
		       "Device %zu (%s, handle 0x%" PRIX64
		       ") has the best score (its score is %llu)",
		       best_idx + 1, vulkan->phys_device_properties.deviceName,
		       vulkan->phys_device, best_score);
	vulkan_get_device_queue_families(vulkan->phys_device,
					 &vulkan->queue_families);
	vulkan_get_swapchain_info(vulkan->phys_device,
				  &vulkan->swapchain_info);

	stbds_arrfree(devices);
	return true;
}

bool vulkan_create_logical_device(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkPhysicalDeviceFeatures phys_device_features = { 0 }; // This will be
							       // used later
	VkDeviceCreateInfo device_create_info = { 0 };
	float queue_priority = 1.0f;
	VkDeviceQueueCreateInfo *queue_create_infos = NULL;
	size_t queue_create_info_count = 1;
	char **required_extensions = vulkan_get_device_extensions();
	VkResult result;

	PURPL_LOG_INFO(purpl_inst->logger, "Creating logical device");

	stbds_arrsetlen(queue_create_infos, queue_create_info_count);

	memset(&queue_create_infos[0], 0, sizeof(VkDeviceQueueCreateInfo));
	queue_create_infos[0].sType =
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_infos[0].queueFamilyIndex =
		vulkan->queue_families.graphics_family;
	queue_create_infos[0].queueCount = 1;
	queue_create_infos[0].pQueuePriorities = &queue_priority;
	if (vulkan->queue_families.graphics_family !=
	    vulkan->queue_families.presentation_family) {
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
			(u32)vulkan->queue_families.presentation_family;
		queue_create_infos[1].queueCount = 1;
		queue_create_infos[1].pQueuePriorities = &queue_priority;
	}

	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pEnabledFeatures = &phys_device_features;
	device_create_info.enabledExtensionCount =
		(u32)stbds_arrlenu(required_extensions);
	device_create_info.ppEnabledExtensionNames =
		(const char *const *)required_extensions;
	device_create_info.queueCreateInfoCount = (u32)queue_create_info_count;
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
	stbds_arrfree(required_extensions);

	PURPL_LOG_INFO(
		purpl_inst->logger,
		"Successfully created logical device with handle 0x%" PRIX64,
		vulkan->device);

	vkGetDeviceQueue(vulkan->device,
			 (u32)vulkan->queue_families.compute_family, 0,
			 &vulkan->compute_queue);
	PURPL_LOG_INFO(purpl_inst->logger,
		       "Retrieved handle 0x%" PRIX64 " for compute queue",
		       vulkan->presentation_queue);
	vkGetDeviceQueue(vulkan->device,
			 (u32)vulkan->queue_families.graphics_family, 0,
			 &vulkan->graphics_queue);
	PURPL_LOG_INFO(purpl_inst->logger,
		       "Retrieved handle 0x%" PRIX64 " for graphics queue",
		       vulkan->graphics_queue);
	vkGetDeviceQueue(vulkan->device,
			 (u32)vulkan->queue_families.presentation_family, 0,
			 &vulkan->presentation_queue);
	PURPL_LOG_INFO(purpl_inst->logger,
		       "Retrieved handle 0x%" PRIX64 " for presentation queue",
		       vulkan->presentation_queue);

	return true;
}

u64 vulkan_score_device(VkPhysicalDevice device, size_t idx)
{
	u64 score = 0;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	struct vulkan_queue_families queue_families = { 0 };
	struct vulkan_swapchain_info swapchain_info = { 0 };
	size_t extension_count;

	if (!device) {
		PURPL_LOG_DEBUG(purpl_inst->logger,
				"device parameter was NULL");
		return 0;
	}

	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);

	if (!vulkan_get_device_queue_families(device, &queue_families)) {
		PURPL_LOG_INFO(
			purpl_inst->logger,
			"Device %zu (%s) does not have the necessary queue families, ignoring",
			idx + 1, properties.deviceName);
		return 0;
	}

	if (!vulkan_check_device_extensions(device, &extension_count)) {
		PURPL_LOG_INFO(
			purpl_inst->logger,
			"Device %zu (%s) does not have the necessary extensions, ignoring",
			idx + 1, properties.deviceName);
		return 0;
	}

	if (!vulkan_get_swapchain_info(device, &swapchain_info)) {
		PURPL_LOG_INFO(
			purpl_inst->logger,
			"Device %zu (%s) does not have the necessary swapchain info, ignoring",
			idx + 1, properties.deviceName);
		return 0;
	}

	if (properties.apiVersion < VK_API_VERSION_1_1) {
		PURPL_LOG_INFO(
			purpl_inst->logger,
			"Device %zu (%s) does not support Vulkan 1.1, ignoring",
			idx + 1, properties.deviceName);
		return 0;
	}
	
	// Give more points for devices that support newer standards
	score += VK_API_VERSION_MINOR(properties.apiVersion) * 100;

	// iGPUs are usually shit, and on a system that doesn't follow that
	// rule (i.e. the Steam Deck or a mobile device) there probably
	// won't be a dGPU
	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	// The Vulkan tutorial said this makes performance better, favour it
	if (queue_families.graphics_family ==
	    queue_families.presentation_family)
		score += 100;

	score += properties.limits.maxImageDimension2D;
	score += extension_count * 10; // More extensions == more better

	PURPL_LOG_INFO(purpl_inst->logger,
		       "Device %zu (handle 0x%" PRIX64 "):", idx + 1, device);
	PURPL_LOG_INFO(purpl_inst->logger, "\tName: %s",
		       properties.deviceName);
	PURPL_LOG_INFO(purpl_inst->logger, "\tScore: %d", score);
	PURPL_LOG_INFO(purpl_inst->logger, "\tSupported extension count: %zu",
		       extension_count);
	PURPL_LOG_INFO(purpl_inst->logger, "\tQueue family indices:");
	PURPL_LOG_INFO(purpl_inst->logger, "\t\tCompute: %zu",
		       queue_families.compute_family);
	PURPL_LOG_INFO(purpl_inst->logger, "\t\tGraphics: %zu",
		       queue_families.graphics_family);
	PURPL_LOG_INFO(purpl_inst->logger, "\t\tPresentation: %zu",
		       queue_families.presentation_family);
	PURPL_LOG_INFO(purpl_inst->logger, "\tSwapchain information:");
	PURPL_LOG_INFO(purpl_inst->logger, "\t\tSurface format count: %zu",
		       stbds_arrlenu(swapchain_info.formats));
	PURPL_LOG_INFO(purpl_inst->logger, "\t\tPresent mode count: %zu",
		       stbds_arrlenu(swapchain_info.present_modes));

	stbds_arrfree(swapchain_info.formats);
	stbds_arrfree(swapchain_info.present_modes);

	return score;
}

char **vulkan_get_device_extensions(void)
{
	char **extensions = NULL;

	stbds_arrput(extensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	return extensions;
}

bool vulkan_check_device_extensions(VkPhysicalDevice device,
				    size_t *extension_count)
{
	// GitHub Copilot autocompleted this entire function (but I had to
	// change it a bit to get more information for the device score)
	u32 extension_count_tmp = 0;
	VkExtensionProperties *extensions = NULL;
	char **extension_names;
	size_t found = 0;
	bool all_found = false;
	size_t i;
	size_t j;

	vkEnumerateDeviceExtensionProperties(device, NULL,
					     &extension_count_tmp, NULL);
	if (!extension_count) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to locate any device extensions");
		return false;
	}

	extension_names = vulkan_get_device_extensions();

	stbds_arrsetlen(extensions, extension_count_tmp);
	vkEnumerateDeviceExtensionProperties(device, NULL,
					     &extension_count_tmp, extensions);

	for (i = 0; i < stbds_arrlenu(extensions); i++) {
		for (j = 0; j < stbds_arrlenu(extension_names); j++) {
			if (strcmp(extensions[i].extensionName,
				   extension_names[j]) == 0) {
				PURPL_LOG_INFO(
					purpl_inst->logger,
					"Found extension %s for device with handle 0x%" PRIX64
					" (total found so far: %zu)",
					extensions[i].extensionName, device,
					found + 1);
				found++;
				break;
			}
		}
	}

	all_found = found >= stbds_arrlenu(extension_names);
	if (all_found)
		PURPL_LOG_INFO(
			purpl_inst->logger,
			"All device extensions (%zu in total) found for device with handle 0x%" PRIX64,
			found, device);

	stbds_arrfree(extensions);
	stbds_arrfree(extension_names);

	if (extension_count)
		*extension_count = extension_count_tmp;
	return all_found;
}

bool vulkan_get_device_queue_families(
	VkPhysicalDevice device, struct vulkan_queue_families *queue_families)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkQueueFamilyProperties *queue_family_list = NULL;
	u32 queue_family_count = 0;
	bool all_found = false;
	size_t i;

	if (!device || !queue_families) {
		PURPL_LOG_DEBUG(purpl_inst->logger,
				"A mandatory parameter was NULL");
		return false;
	}

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
						 NULL);
	stbds_arrsetlen(queue_family_list, queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
						 queue_family_list);

	for (i = 0; i < stbds_arrlenu(queue_family_list); i++) {
		if (queue_family_list[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			queue_families->compute_family_present = true;
			queue_families->compute_family = i;
		}

		if (queue_family_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queue_families->graphics_family_present = true;
			queue_families->graphics_family = i;
		}

		VkBool32 presentation_support = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(
			device, i, vulkan->surface, &presentation_support);
		if (presentation_support) {
			queue_families->presentation_family_present = true;
			queue_families->presentation_family = i;
		}

		all_found = (queue_families->compute_family_present &&
			     queue_families->graphics_family_present &&
			     queue_families->presentation_family_present);
		if (all_found)
			break;
	}

	return all_found;
}
