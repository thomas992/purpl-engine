// Debugging functions for Vulkan (primarily callbacks)
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

#include "purpl/graphics/vulkan/debug.h"

/// Callback for the debug messenger
///
/// \param severity The severity of the message
/// \param type The type of message
/// \param callback_data Data such as the message and objects related to it
/// \param data Data from the engine, not likely to be needed
static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_log(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *data);

void vulkan_create_debug_messenger(void)
{
	VkDebugUtilsMessengerCreateInfoEXT create_info;

	create_info.sType =
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = vulkan_debug_log;
	create_info.pUserData = NULL;


}

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_log(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *data)
{
	enum purpl_log_level level;

	switch (severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		level = PURPL_LOG_LEVEL_DEBUG;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		level = PURPL_LOG_LEVEL_INFO;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		level = PURPL_LOG_LEVEL_WARNING;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		level = PURPL_LOG_LEVEL_ERROR;
		break;
	}

	purpl_log_write(purpl_inst->logger, level, __FILE__, __LINE__,
			PURPL_CURRENT_FUNCTION,
			"Vulkan debug messenger (%zu objects involved): %s",
			callback_data->objectCount, callback_data->pMessage);

	return false;
}
