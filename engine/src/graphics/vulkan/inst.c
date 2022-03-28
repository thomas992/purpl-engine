// Functions for creating Vulkan instances
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

#include "purpl/graphics/vulkan/inst.h"

bool vulkan_create_instance(void)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;
	VkResult res = 0;
	VkInstanceCreateInfo create_info = { 0 };
	VkApplicationInfo app_info = { 0 };
	void **exts;
	char **required_exts;
	VkExtensionProperties *ext_props;
	size_t ext_count = 0;
	size_t layer_count = 0;

	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = purpl_inst->app_name;
	app_info.applicationVersion = VK_MAKE_VERSION(
		PURPL_GET_MAJOR_VERSION(purpl_inst->app_version),
		PURPL_GET_MINOR_VERSION(purpl_inst->app_version),
		PURPL_GET_PATCH_VERSION(purpl_inst->app_version));
	app_info.pEngineName = "Purpl Engine";
	app_info.engineVersion = VK_MAKE_VERSION(
		PURPL_MAJOR_VERSION, PURPL_MINOR_VERSION, PURPL_PATCH_VERSION);
	app_info.apiVersion = VK_API_VERSION_1_3;

	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	exts = vulkan_get_extensions();
	required_exts = exts[0];
	ext_props = exts[1];

	create_info.enabledExtensionCount = stbds_arrlenu(required_exts);
	create_info.ppEnabledExtensionNames = required_exts;
	ext_count = stbds_arrlenu(required_exts);

#ifdef PURPL_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { 0 };
	size_t i;

	const char *validation_layers[] = { "VK_LAYER_KHRONOS_validation" };
	layer_count = PURPL_SIZEOF_ARRAY(validation_layers);

	create_info.enabledLayerCount = PURPL_SIZEOF_ARRAY(validation_layers);
	create_info.ppEnabledLayerNames = validation_layers;
	for (i = 0; i < PURPL_SIZEOF_ARRAY(validation_layers); i++)
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Requesting Vulkan validation layer \"%s\"",
			       validation_layers[i]);

	vulkan_setup_debug_messenger(&debug_create_info);

	// Vulkan will create this if it's put here, without having to load
	// functions
	create_info.pNext = &debug_create_info;
#else // PURPL_DEBUG
	create_info.enabledLayerCount = 0;
#endif // PURPL_DEBUG

	res = vkCreateInstance(&create_info, NULL, &vulkan->inst);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to create Vulkan instance: VkResult %d", res);
		stbds_arrfree(required_exts);
		stbds_arrfree(ext_props);
		stbds_arrfree(exts);
		return false;
	}

	PURPL_LOG_INFO(
		purpl_inst->logger,
		"Successfully created a Vulkan instance with %zu extension%s and %zu validation layer%s enabled",
		ext_count, ext_count == 1 ? "" : "s", layer_count,
		layer_count == 1 ? "" : "s");

	stbds_arrfree(required_exts);
	stbds_arrfree(ext_props);
	stbds_arrfree(exts);

	return true;
}

void **vulkan_get_extensions(void)
{
	void **exts = NULL;
	char **required_exts = NULL;
	u32 ext_count = 0;
	VkExtensionProperties *ext_props = NULL;
	u32 i;

	SDL_Vulkan_GetInstanceExtensions(purpl_inst->wnd, &ext_count, NULL);
	stbds_arrsetlen(required_exts, ext_count);
	SDL_Vulkan_GetInstanceExtensions(purpl_inst->wnd, &ext_count,
					 required_exts);

	ext_count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &ext_count, NULL);
	stbds_arrsetlen(ext_props, ext_count);
	vkEnumerateInstanceExtensionProperties(NULL, &ext_count, ext_props);
	for (i = 0; i < stbds_arrlenu(ext_props); i++) {
		PURPL_LOG_INFO(
			purpl_inst->logger,
			"Found Vulkan instance extension \"%s\" version %d",
			ext_props[i].extensionName, ext_props[i].specVersion);

#ifdef PURPL_DEBUG
		if (strcmp(ext_props[i].extensionName,
			   VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0 &&
		    ext_props[i].specVersion ==
			    VK_EXT_DEBUG_UTILS_SPEC_VERSION)
			stbds_arrput(required_exts,
				     VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // PURPL_DEBUG
	}

	for (i = 0; i < stbds_arrlenu(required_exts); i++)
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Requesting Vulkan instance extension \"%s\"",
			       required_exts[i]);

	stbds_arrput(exts, required_exts);
	stbds_arrput(exts, ext_props);

	return exts;
}
