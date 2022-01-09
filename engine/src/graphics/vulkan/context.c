// purpl/graphics/vulkan/context.h implementations
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

#include "purpl/core/inst.h"

#include "purpl/graphics/vulkan/context.h"

bool purpl_vulkan_create_context(struct purpl_vulkan_context *context)
{
	VkApplicationInfo app_info = { 0 };
	VkInstanceCreateInfo create_info = { 0 };
	VkResult result;
	u32 extension_count;
	const char **extension_names;
	u32 i;

	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = purpl_inst->app_name;
	app_info.applicationVersion = PURPL_VERSION_TO_VK_VERSION(purpl_inst->app_version);
	app_info.pEngineName = "Purpl Engine";
	app_info.engineVersion = PURPL_VERSION_TO_VK_VERSION(PURPL_VERSION);
	app_info.apiVersion = VK_API_VERSION_1_2;

	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	PURPL_LOG_INFO(purpl_inst->logger, "Getting required Vulkan extensions");

	SDL_Vulkan_GetInstanceExtensions(purpl_inst->wnd, &extension_count, NULL);

	extension_names = calloc(extension_count, sizeof(const char *));
	if (!extension_names) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to get required Vulkan extensions: %s", purpl_strerror());
		return false;
	}

	SDL_Vulkan_GetInstanceExtensions(purpl_inst->wnd, &extension_count, extension_names);

	PURPL_LOG_INFO(purpl_inst->logger, "Found %d required extensions:", extension_count);
	for (i = 0; i < extension_count; i++)
		PURPL_LOG_INFO(purpl_inst->logger, "Extension %d: %s", i + 1, extension_names[i]);

	create_info.enabledExtensionCount = extension_count;
	create_info.ppEnabledExtensionNames = extension_names;

	create_info.enabledLayerCount = 0;

	result = vkCreateInstance(&create_info, NULL, &context->inst);
	if (result != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to create a Vulkan instance: %s", purpl_vulkan_strerror(result));
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Successfully created a Vulkan instance");

	return true;
}

void purpl_vulkan_destroy_context(struct purpl_vulkan_context *context)
{

}
