// Vulkan stuff

#pragma once

#include "vk_glad.h"
#include "SDL_vulkan.h"

// Vulkan initialization
extern bool engine_vulkan_init(void);

// Choose a physical device
extern bool engine_vulkan_choose_device(void);

// Create a logical device
extern bool engine_vulkan_create_device(void);

// Prepare to draw a frame
extern bool engine_vulkan_begin_frame(uint64_t delta);

// Finish commands
extern bool engine_vulkan_end_frame(uint64_t delta);

// Shut down Vulkan
extern void engine_vulkan_shutdown(void);

// Vulkan instance
extern VkInstance g_vulkan_inst;

// Physical and logical devices
extern VkPhysicalDevice g_vulkan_phys_device;
extern VkDevice g_vulkan_device;
