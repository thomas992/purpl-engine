// Swapchain related things
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

#include "stb_ds.h"

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/log.h"
#include "purpl/core/types.h"

#include "purpl/util/util.h"

#include "pipeline.h"
#include "render.h"

/// Create a swapchain
///
/// \return Returns true if the swapchain was created successfully, false otherwise
extern bool vulkan_create_swapchain(void);

/// Recreate the swapchain
///
/// \return Returns true if successful, false if not
extern bool vulkan_recreate_swapchain(void);

/// Destroy the swapchain and related information
extern void vulkan_destroy_swapchain(void);

/// Create image views for the swapchain images
///
/// \return Returns true if the image views were created successfully, false otherwise
extern bool vulkan_create_image_views(void);

/// Destroy the image views
extern void vulkan_destroy_image_views(void);

/// Determine if the given device is supported for swapchain creation
///
/// \param device The device to check
/// \param info Receives the swapchain information
///
/// \returns Returns true if the device is supported, false otherwise
extern bool vulkan_get_swapchain_info(VkPhysicalDevice device, struct vulkan_swapchain_info *info);

/// Choose the best surface format for the swapchain
///
/// \return Returns the best surface format
extern VkSurfaceFormatKHR *vulkan_choose_swap_surface_format(void);

/// Choose the best presentation mode for the swapchain
///
/// \return Returns the best presentation mode
extern VkPresentModeKHR vulkan_choose_swap_present_mode(void);

/// Choose the size for the swapchain images
///
/// \return Returns the extent of the images
extern VkExtent2D vulkan_choose_swap_extent(void);
