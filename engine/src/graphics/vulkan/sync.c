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

bool vulkan_create_semaphores()
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

	PURPL_LOG_INFO(purpl_inst->logger, "Successfully created presentation semaphore with handle 0x%" PRIX64, vulkan->present_semaphore);

	res = vkCreateSemaphore(vulkan->device, &semaphore_create_info, NULL, &vulkan->render_semaphore);
	if (res != VK_SUCCESS) {
		PURPL_LOG_INFO(purpl_inst->logger, "Failed to create render semaphore: VkResult %d", res);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Successfully created render semaphore with handle 0x%" PRIX64, vulkan->render_semaphore);

	return true;
}

bool vulkan_create_render_fence()
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);


}

