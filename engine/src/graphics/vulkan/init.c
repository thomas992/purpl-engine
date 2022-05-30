// Vulkan initialization
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

#include "glad/vulkan.h"

#include "purpl/graphics/vulkan/init.h"

bool purpl_vulkan_init(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

#ifdef __APPLE__
	return false;
#endif // __APPLE__

	if (!purpl_inst)
		return false;

	purpl_inst->graphics_api = PURPL_GRAPHICS_API_VULKAN;

	purpl_inst->wnd_title = purpl_get_initial_window_title();
	PURPL_LOG_INFO(purpl_inst->logger, "Creating a window titled \"%s\"", purpl_inst->wnd_title);
	purpl_inst->wnd = SDL_CreateWindow(purpl_inst->wnd_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					   PURPL_INITIAL_WINDOW_WIDTH, PURPL_INITIAL_WINDOW_HEIGHT,
					   PURPL_DEFAULT_WINDOW_FLAGS | SDL_WINDOW_VULKAN);
	if (!purpl_inst->wnd) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to create window: %s", SDL_GetError());
		purpl_vulkan_shutdown();
		return false;
	}

	SDL_HideWindow(purpl_inst->wnd);

	if (!vulkan_create_instance()) {
		purpl_vulkan_shutdown();
		return false;
	}

	gladLoaderLoadVulkan(vulkan->inst, NULL, NULL);

	if (!vulkan_create_surface()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_pick_physical_device()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_logical_device()) {
		purpl_vulkan_shutdown();
		return false;
	}

	gladLoaderLoadVulkan(vulkan->inst, vulkan->phys_device, vulkan->device);

	if (!vulkan_create_swapchain()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_command_pool()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_alloc_command_buffer()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_default_renderpass()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_pipeline()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_framebuffers()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_sync_objs()) {
		purpl_vulkan_shutdown();
		return false;
	}

	return true;
}

bool purpl_vulkan_update(u64 delta)
{
	PURPL_IGNORE(delta);

	return vulkan_draw();
}

void purpl_vulkan_shutdown(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	vulkan_destroy_sync_objs();

	vulkan_destroy_framebuffers();

	if (vulkan->renderpass) {
		vkDestroyRenderPass(vulkan->device, vulkan->renderpass, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed default renderpass with handle 0x%" PRIX64, vulkan->renderpass);
	}

	if (vulkan->cmd_pool) {
		vkDestroyCommandPool(vulkan->device, vulkan->cmd_pool, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed command pool with handle 0x%" PRIX64, vulkan->cmd_pool);
	}

	vulkan_destroy_swapchain();

	if (vulkan->device) {
		vkDestroyDevice(vulkan->device, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed logical device with handle 0x%" PRIX64, vulkan->device);
	}

	if (vulkan->surface) {
		vkDestroySurfaceKHR(vulkan->inst, vulkan->surface, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed surface with handle 0x%" PRIX64, vulkan->surface);
	}

	if (vulkan->inst) {
		vkDestroyInstance(vulkan->inst, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed Vulkan instance with handle 0x%" PRIX64, vulkan->device);
	}

	purpl_inst->graphics_api = PURPL_GRAPHICS_API_MAX;
}
