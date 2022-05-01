// Vulkan command stuff
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

#include "purpl/graphics/vulkan/command.h"

bool vulkan_create_command_pool(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);
	
	VkCommandPoolCreateInfo cmd_pool_create_info = { 0 };
	VkResult res;

	PURPL_LOG_INFO(purpl_inst->logger, "Creating command pool");

	cmd_pool_create_info.sType =
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_create_info.queueFamilyIndex =
		vulkan->queue_families.graphics_family;
	cmd_pool_create_info.flags =
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	
	res = vkCreateCommandPool(vulkan->device, &cmd_pool_create_info, NULL,
				  &vulkan->cmd_pool);
	if (res != VK_SUCCESS) {;
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to create command pool: VkResult %d", res);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Successfully created command pool");

	return true;
}
