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

#include "SDL_vulkan.h"

#include "purpl/graphics/vulkan/swapchain.h"

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

	u32 queue_families[] = { vulkan->queue_families.graphics_family, vulkan->queue_families.presentation_family };

	PURPL_LOG_INFO(purpl_inst->logger, "Creating swapchain");

	surface_format = vulkan_choose_swap_surface_format();
	present_mode = vulkan_choose_swap_present_mode();
	extent = vulkan_choose_swap_extent();

	image_count = vulkan->swapchain_info.capabilities.maxImageCount > 0 ?
			      vulkan->swapchain_info.capabilities.maxImageCount :
				    vulkan->swapchain_info.capabilities.minImageCount + 1;

	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = vulkan->surface;
	swapchain_create_info.minImageCount = image_count;
	swapchain_create_info.imageFormat = surface_format->format;
	swapchain_create_info.imageColorSpace = surface_format->colorSpace;
	swapchain_create_info.imageExtent = extent;
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (queue_families[0] != queue_families[1]) {
		swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_create_info.queueFamilyIndexCount = 2;
		swapchain_create_info.pQueueFamilyIndices = queue_families;
	} else {
		// Other fields are already zeroed
		swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapchain_create_info.preTransform = vulkan->swapchain_info.capabilities.currentTransform;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.presentMode = present_mode;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.oldSwapchain = vulkan->swapchain; // Because of
								// calloc, this
								// will be NULL
								// the first
								// time

	result = vkCreateSwapchainKHR(vulkan->device, &swapchain_create_info, NULL, &vulkan->swapchain);
	if (result != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to create swapchain: VkResult %d", result);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Created swapchain with handle 0x%" PRIx64, vulkan->swapchain);

	PURPL_LOG_INFO(purpl_inst->logger, "Getting swapchain image handles");

	vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapchain, &image_count, NULL);
	stbds_arrsetlen(vulkan->swapchain_images, image_count);
	vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapchain, &image_count, vulkan->swapchain_images);

	PURPL_LOG_INFO(purpl_inst->logger, "Got %zu swapchain image handles from swapchain with handle 0x%" PRIx64,
		       stbds_arrlenu(vulkan->swapchain_images), vulkan->swapchain);
	for (i = 0; i < image_count; i++)
		PURPL_LOG_INFO(purpl_inst->logger, "Swapchain image %zu has handle 0x%" PRIx64, i + 1,
			       vulkan->swapchain_images[i]);

	vulkan->swapchain_format = surface_format->format;
	vulkan->swapchain_extent = extent;

	return true;
}

void vulkan_destroy_swapchain(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	if (vulkan->swapchain_images) {
		stbds_arrfree(vulkan->swapchain_images);
		PURPL_LOG_INFO(purpl_inst->logger, "Freed swapchain image handle array");
	}

	if (vulkan->swapchain_info.formats) {
		stbds_arrfree(vulkan->swapchain_info.formats);
		PURPL_LOG_INFO(purpl_inst->logger, "Freed swapchain format array");
	}

	if (vulkan->swapchain_info.present_modes) {
		stbds_arrfree(vulkan->swapchain_info.present_modes);
		PURPL_LOG_INFO(purpl_inst->logger, "Freed swapchain present mode array");
	}

	if (vulkan->swapchain) {
		vkDestroySwapchainKHR(vulkan->device, vulkan->swapchain, NULL);
		PURPL_LOG_INFO(purpl_inst->logger, "Destroyed swapchain with handle 0x%" PRIx64, vulkan->swapchain);
	}
}

bool vulkan_create_image_views(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkResult result;
	VkImageViewCreateInfo image_view_create_info = { 0 };
	size_t i;

	PURPL_LOG_INFO(purpl_inst->logger, "Creating %zu image views", stbds_arrlenu(vulkan->swapchain_images));

	stbds_arrsetlen(vulkan->swapchain_image_views, stbds_arrlenu(vulkan->swapchain_images));

	for (i = 0; i < stbds_arrlenu(vulkan->swapchain_images); i++) {
		image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_create_info.image = vulkan->swapchain_images[i];

		image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		image_view_create_info.format = vulkan->swapchain_format;

		image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_create_info.subresourceRange.baseMipLevel = 0;
		image_view_create_info.subresourceRange.levelCount = 1;
		image_view_create_info.subresourceRange.baseArrayLayer = 0;
		image_view_create_info.subresourceRange.layerCount = 1;

		result = vkCreateImageView(vulkan->device, &image_view_create_info, NULL,
					   &vulkan->swapchain_image_views[i]);
		if (result != VK_SUCCESS) {
			PURPL_LOG_ERROR(purpl_inst->logger,
					"Failed to create image view for image %zu with handle 0x%" PRIX64
					": VkResult %d",
					i + 1, vulkan->swapchain_images[i], result);
			vulkan_destroy_image_views();
			return false;
		}
	}

	return true;
}

void vulkan_destroy_image_views(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	size_t i;

	if (vulkan->swapchain_image_views) {
		for (i = 0; i < stbds_arrlenu(vulkan->swapchain_image_views); i++) {
			if (vulkan->swapchain_image_views[i]) {
				vkDestroyImageView(vulkan->device, vulkan->swapchain_image_views[i], NULL);
				PURPL_LOG_INFO(purpl_inst->logger,
					       "Destroyed image view for image %zu with handle 0x%" PRIX64, i + 1,
					       vulkan->swapchain_images[i]);
			}
		}

		stbds_arrfree(vulkan->swapchain_image_views);
		PURPL_LOG_INFO(purpl_inst->logger, "Freed swapchain image view array");
	}
}

bool vulkan_get_swapchain_info(VkPhysicalDevice device, struct vulkan_swapchain_info *info)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	u32 format_count = 0;
	u32 present_mode_count = 0;

	if (!info) {
		PURPL_LOG_DEBUG(purpl_inst->logger, "info is NULL, returning");
		return false;
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkan->surface, &info->capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->surface, &format_count, NULL);
	if (!format_count) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to get surface formats");
		return false;
	}
	stbds_arrsetlen(info->formats, format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->surface, &format_count, info->formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->surface, &present_mode_count, NULL);
	if (!present_mode_count) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to get surface present modes");
		return false;
	}
	stbds_arrsetlen(info->present_modes, present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->surface, &present_mode_count, info->present_modes);

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

	for (i = 0; i < stbds_arrlenu(vulkan->swapchain_info.present_modes); i++) {
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
	u32 width;
	u32 height;

	SDL_Vulkan_GetDrawableSize(purpl_inst->wnd, (s32 *)&width, (s32 *)&height);
	extent.width = PURPL_CLAMP(width, vulkan->swapchain_info.capabilities.minImageExtent.width,
				   vulkan->swapchain_info.capabilities.maxImageExtent.width);
	extent.height = PURPL_CLAMP(height, vulkan->swapchain_info.capabilities.minImageExtent.height,
				    vulkan->swapchain_info.capabilities.maxImageExtent.height);

	PURPL_LOG_INFO(purpl_inst->logger, "Chose swap extent %ux%u", extent.width, extent.height);
	return extent;
}
