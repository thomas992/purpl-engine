// Swapchain related functions
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

#include "purpl/graphics/vulkan/swapchain.h"
#include "SDL_vulkan.h"
#include <vulkan/vulkan_core.h>

bool vulkan_create_swapchain(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkSwapchainCreateInfoKHR swapchain_create_info = { 0 };
	VkSurfaceFormatKHR *surface_format;
	VkPresentModeKHR present_mode;
	VkExtent2D extent;
	u32 image_count;
	VkResult result;
	size_t i;

	u32 queue_families[] = { vulkan->queue_families.graphics_family,
				 vulkan->queue_families.presentation_family };

	PURPL_LOG_INFO(purpl_inst->logger, "Creating swapchain");

	surface_format = vulkan_choose_swap_surface_format();
	present_mode = vulkan_choose_swap_present_mode();
	extent = vulkan_choose_swap_extent();

	image_count =
		vulkan->swapchain_info.capabilities.maxImageCount > 0 ?
			vulkan->swapchain_info.capabilities.maxImageCount :
			      vulkan->swapchain_info.capabilities.minImageCount + 1;

	swapchain_create_info.sType =
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = vulkan->surface;
	swapchain_create_info.minImageCount = image_count;
	swapchain_create_info.imageFormat = surface_format->format;
	swapchain_create_info.imageColorSpace = surface_format->colorSpace;
	swapchain_create_info.imageExtent = extent;
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (queue_families[0] != queue_families[1]) {
		swapchain_create_info.imageSharingMode =
			VK_SHARING_MODE_CONCURRENT;
		swapchain_create_info.queueFamilyIndexCount = 2;
		swapchain_create_info.pQueueFamilyIndices = queue_families;
	} else {
		// Other fields are already zeroed
		swapchain_create_info.imageSharingMode =
			VK_SHARING_MODE_EXCLUSIVE;
	}

	swapchain_create_info.preTransform =
		vulkan->swapchain_info.capabilities.currentTransform;
	swapchain_create_info.compositeAlpha =
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.presentMode = present_mode;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.oldSwapchain = vulkan->swapchain; // Because of
								// calloc, this
								// will be NULL
								// the first
								// time

	result = vkCreateSwapchainKHR(vulkan->device, &swapchain_create_info,
				      NULL, &vulkan->swapchain);
	if (result != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to create swapchain: VkResult %d",
				result);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger,
		       "Created swapchain with handle 0x%" PRIx64,
		       vulkan->swapchain);

	PURPL_LOG_INFO(purpl_inst->logger, "Getting swapchain image handles");

	vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapchain,
				&image_count, NULL);
	stbds_arrsetlen(vulkan->swapchain_images, image_count);
	vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapchain,
				&image_count, vulkan->swapchain_images);

	PURPL_LOG_INFO(purpl_inst->logger, "Got %zu swapchain images",
		       stbds_arrlenu(vulkan->swapchain_images));
	for (i = 0; i < image_count; i++)
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Swapchain image %zu has handle 0x%" PRIx64,
			       i + 1, vulkan->swapchain_images[i]);

	vulkan->swapchain_format = surface_format->format;
	vulkan->swapchain_extent = extent;

	return true;
}

void vulkan_destroy_swapchain(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	if (vulkan->swapchain_images) {
		stbds_arrfree(vulkan->swapchain_images);
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Freed swapchain image handle array");
	}

	if (vulkan->swapchain_info.formats) {
		stbds_arrfree(vulkan->swapchain_info.formats);
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Freed swapchain format array");
	}

	if (vulkan->swapchain_info.present_modes) {
		stbds_arrfree(vulkan->swapchain_info.present_modes);
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Freed swapchain present mode array");
	}

	if (vulkan->swapchain) {
		vkDestroySwapchainKHR(vulkan->device, vulkan->swapchain, NULL);
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Destroyed swapchain with handle 0x%" PRIx64,
			       vulkan->swapchain);
	}
}

bool vulkan_get_swapchain_info(VkPhysicalDevice device,
			       struct vulkan_swapchain_info *info)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	u32 format_count = 0;
	u32 present_mode_count = 0;

	if (!info) {
		PURPL_LOG_DEBUG(purpl_inst->logger, "info is NULL, returning");
		return false;
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkan->surface,
						  &info->capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->surface,
					     &format_count, NULL);
	if (!format_count) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to get surface formats");
		return false;
	}
	stbds_arrsetlen(info->formats, format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->surface,
					     &format_count, info->formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->surface,
						  &present_mode_count, NULL);
	if (!present_mode_count) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to get surface present modes");
		return false;
	}
	stbds_arrsetlen(info->present_modes, present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->surface,
						  &present_mode_count,
						  info->present_modes);

	return true;
}

VkSurfaceFormatKHR *vulkan_choose_swap_surface_format(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkSurfaceFormatKHR *format;
	size_t i;

	for (i = 0; i < stbds_arrlenu(vulkan->swapchain_info.formats); i++) {
		format = &vulkan->swapchain_info.formats[i];
		// sRGB is good
		if (format->format == VK_FORMAT_B8G8R8A8_SRGB &&
		    format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;
	}

	// Return the first format if the sRGB one couldn't be found
	return &vulkan->swapchain_info.formats[0];
}

VkPresentModeKHR vulkan_choose_swap_present_mode(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkPresentModeKHR present_mode;
	size_t i;

	for (i = 0; i < stbds_arrlenu(vulkan->swapchain_info.present_modes);
	     i++) {
		present_mode = vulkan->swapchain_info.present_modes[i];
		if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return present_mode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vulkan_choose_swap_extent(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkExtent2D extent;
	int32_t width;
	int32_t height;

	SDL_Vulkan_GetDrawableSize(purpl_inst->wnd, &width, &height);
	extent.width = PURPL_CLAMP(
		width,
		vulkan->swapchain_info.capabilities.minImageExtent.width,
		vulkan->swapchain_info.capabilities.maxImageExtent.width);
	extent.height = PURPL_CLAMP(
		height,
		vulkan->swapchain_info.capabilities.minImageExtent.height,
		vulkan->swapchain_info.capabilities.maxImageExtent.height);

	return extent;
}
