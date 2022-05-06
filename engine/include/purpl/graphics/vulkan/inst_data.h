// Vulkan data
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

#pragma once

#include <vulkan/vulkan.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

/// Vulkan queue family information
struct vulkan_queue_families {
	size_t compute_family; /// The index of the compute queue
			       /// family
	bool compute_family_present; /// Whether compute_family is
				     /// present
	size_t graphics_family; /// The index of the graphics queue family
	bool graphics_family_present; /// Whether graphics_family is present
	size_t presentation_family; /// The index of the presentation queue
				    /// family
	bool presentation_family_present; /// Whether presentation_family is
					  /// present
};

/// Vulkan swapchain information
struct vulkan_swapchain_info {
	VkSurfaceCapabilitiesKHR capabilities; /// The capabilities of the swapchain
					       /// surface
	VkSurfaceFormatKHR *formats; /// The formats of the surface
	VkPresentModeKHR *present_modes; /// The presentation modes of the surface
};

struct purpl_instance_vulkan {
	VkInstance inst; /// Vulkan instance
	VkDebugUtilsMessengerEXT debug_messenger; /// Debug messenger

	VkPhysicalDevice phys_device; /// Physical device
	VkPhysicalDeviceFeatures phys_device_features; /// Physical device
						       /// features
	VkPhysicalDeviceProperties phys_device_properties; /// Physical device
							   /// properties

	struct vulkan_queue_families queue_families; /// The queue
						     /// families to
						     /// use with
						     /// phys_device
	VkDevice device; /// Logical device

	VkSurfaceKHR surface; /// Surface

	VkQueue compute_queue; /// Compute queue
	VkQueue graphics_queue; /// Graphics queue
	VkQueue presentation_queue; /// Presentation queue

	struct vulkan_swapchain_info swapchain_info; /// Swapchain information
	VkSwapchainKHR swapchain; /// Swapchain
	VkImage *swapchain_images; /// Swapchain images
	VkFormat swapchain_format; /// Swapchain format
	VkExtent2D swapchain_extent; /// Swapchain extent
	VkImageView *swapchain_image_views; /// Swapchain image views

	VkCommandPool cmd_pool; /// Command pool
	VkCommandBuffer cmd_buf; /// Command buffer

	VkRenderPass renderpass; /// Render pass
	VkFramebuffer *framebuffers; /// Framebuffers
};
