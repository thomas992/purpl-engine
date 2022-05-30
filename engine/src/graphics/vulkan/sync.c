//  Synchronization things
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

#include "purpl/graphics/vulkan/sync.h"

bool vulkan_create_sync_objs(void)
{
	PURPL_LOG_INFO(purpl_inst->logger, "Creating synchronization objects");

	if (!vulkan_create_render_fence() || !vulkan_create_semaphores()) {
		PURPL_LOG_INFO(purpl_inst->logger, "Failed to create synchronization objects");
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Created synchronization objects");

	return true;
}

void vulkan_destroy_sync_objs(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	PURPL_LOG_INFO(purpl_inst->logger, "Destroying synchronization objects");

	if (vulkan->render_fence) {
		PURPL_LOG_INFO(purpl_inst->logger, "Waiting for render fence with handle 0x%" PRIX64 " before destroying it", vulkan->render_fence);
		vkWaitForFences(vulkan->device, 1, &vulkan->render_fence, true, UINT64_MAX);
		vkDestroyFence(vulkan->device, vulkan->render_fence, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed render fence with handle 0x%" PRIX64, vulkan->render_fence);
	}

	if (vulkan->render_semaphore) {
		vkDestroySemaphore(vulkan->device, vulkan->render_semaphore, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed render semaphore with handle 0x%" PRIX64, vulkan->render_semaphore);
	}

	if (vulkan->present_semaphore) {
		vkDestroySemaphore(vulkan->device, vulkan->present_semaphore, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed presentation semaphore with handle 0x%" PRIX64, vulkan->present_semaphore);
	}
}

bool vulkan_create_semaphores(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkResult res;

	VkSemaphoreCreateInfo semaphore_create_info = { 0 };

	PURPL_LOG_INFO(purpl_inst->logger, "Creating semaphores");

	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	res = vkCreateSemaphore(vulkan->device, &semaphore_create_info, NULL, &vulkan->present_semaphore);
	if (res != VK_SUCCESS) {
		PURPL_LOG_INFO(purpl_inst->logger, "Failed to create presentation semaphore: VkResult %d", res);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Created presentation semaphore with handle 0x%" PRIX64, vulkan->present_semaphore);

	res = vkCreateSemaphore(vulkan->device, &semaphore_create_info, NULL, &vulkan->render_semaphore);
	if (res != VK_SUCCESS) {
		PURPL_LOG_INFO(purpl_inst->logger, "Failed to create render semaphore: VkResult %d", res);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Created render semaphore with handle 0x%" PRIX64, vulkan->render_semaphore);

	return true;
}

bool vulkan_create_render_fence(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkFenceCreateInfo fence_create_info = { 0 };
	VkResult res;

	PURPL_LOG_INFO(purpl_inst->logger, "Creating render fence");

	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	
	// This allows waiting on the fence in the first frame, before it's used
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	res = vkCreateFence(vulkan->device, &fence_create_info, NULL, &vulkan->render_fence);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to create render fence: VkResult %d", res);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Created render fence with handle 0x%" PRIX64, vulkan->render_fence);

	return true;
}

