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

#include "SDL.h"

#include "purpl/graphics/vulkan/init.h"

bool purpl_vulkan_init(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

#ifdef __APPLE__
	return false;
#endif // __APPLE__

	if (!purpl_inst)
		return false;

	// The window is created here because SDL doesn't support OpenGL and
	// Vulkan in the same window
	purpl_inst->wnd_title = purpl_get_initial_window_title();

	PURPL_LOG_INFO(purpl_inst->logger, "Creating a window titled \"%s\"",
		       purpl_inst->wnd_title);
	purpl_inst->wnd = SDL_CreateWindow(
		purpl_inst->wnd_title, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, PURPL_INITIAL_WINDOW_WIDTH,
		PURPL_INITIAL_WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
	if (!purpl_inst->wnd) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_instance()) {
		purpl_vulkan_shutdown();
		return false;
	}

	gladLoaderLoadVulkan(vulkan->inst, NULL, NULL);

	// Surface creation is only one line (excluding error checking, which
	// is about the same here as it would be in a separate function), it
	// really doesn't need its own file or function
	PURPL_LOG_INFO(purpl_inst->logger, "Creating surface");
	if (!SDL_Vulkan_CreateSurface(purpl_inst->wnd, vulkan->inst,
				      &vulkan->surface)) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to create surface: %s",
				SDL_GetError());
		purpl_vulkan_shutdown();
		return false;
	}
	PURPL_LOG_INFO(purpl_inst->logger,
		       "Successfully created surface with handle 0x%" PRIX64,
		       vulkan->surface);

	if (!vulkan_pick_physical_device()) {
		purpl_vulkan_shutdown();
		return false;
	}

	if (!vulkan_create_logical_device()) {
		purpl_vulkan_shutdown();
		return false;
	}

	gladLoaderLoadVulkan(vulkan->inst, vulkan->phys_device,
			     vulkan->device);

	purpl_inst->graphics_api = PURPL_GRAPHICS_API_VULKAN;
	return true;
}

void purpl_vulkan_shutdown(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	if (vulkan->device) {
		vkDestroyDevice(vulkan->device, NULL);
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Destroyed logical device with handle 0x%" PRIX64,
			       vulkan->device);
	}

	if (vulkan->surface) {
		vkDestroySurfaceKHR(vulkan->inst, vulkan->surface, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed surface with handle 0x%" PRIX64,
			       vulkan->surface);
	}

	if (vulkan->inst) {
		vkDestroyInstance(vulkan->inst, NULL);
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Destroyed Vulkan instance with handle 0x%" PRIX64,
			       vulkan->device);
	}
}
