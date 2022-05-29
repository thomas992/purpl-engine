// Surface functions
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

#include "purpl/graphics/vulkan/surface.h"

bool vulkan_create_surface(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	PURPL_LOG_INFO(purpl_inst->logger, "Creating surface");

	purpl_mutex_lock(purpl_inst->wnd_mutex);
	
	if (!SDL_Vulkan_CreateSurface(purpl_inst->wnd, vulkan->inst, &vulkan->surface)) {
		purpl_mutex_unlock(purpl_inst->wnd_mutex);
	
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to create surface: %s", SDL_GetError());
		return false;
	}
	
	purpl_mutex_unlock(purpl_inst->wnd_mutex);
	
	PURPL_LOG_INFO(purpl_inst->logger, "Created surface with handle 0x%" PRIX64, vulkan->surface);

	return true;
}

bool vulkan_recreate_surface(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	PURPL_LOG_INFO(purpl_inst->logger, "Recreating surface with handle 0x%" PRIX64, vulkan->surface);

	vkDeviceWaitIdle(vulkan->device);

	vulkan_destroy_framebuffers();
	vkDestroyRenderPass(vulkan->device, vulkan->renderpass, NULL);
	PURPL_LOG_INFO(purpl_inst->logger, "Destroyed renderpass with handle 0x%" PRIX64, vulkan->renderpass);
	vulkan_destroy_image_views();
	vulkan_destroy_swapchain();
	vkDestroySurfaceKHR(vulkan->inst, vulkan->surface, NULL);
	PURPL_LOG_INFO(purpl_inst->logger, "Destroyed surface with handle 0x%" PRIX64, vulkan->surface);
	
	if (!vulkan_create_surface() || !vulkan_get_swapchain_info(vulkan->phys_device, &vulkan->swapchain_info) || !vulkan_create_swapchain() ||
	    !vulkan_create_default_renderpass() || !vulkan_create_pipeline() || !vulkan_create_framebuffers())
		return false;

	PURPL_LOG_INFO(purpl_inst->logger, "Surface recreated with new handle 0x%" PRIX64, vulkan->surface);

	return true;
}

