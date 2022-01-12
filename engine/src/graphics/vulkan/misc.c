// Vulkan utility function implementations
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

#include "purpl/graphics/vulkan/misc.h"

const char *purpl_vulkan_strerror(VkResult result)
{
	static char buf[PURPL_STATIC_BUF_MAX];

	switch (result) {
	case VK_SUCCESS:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX, "Success");
		break;
	case VK_NOT_READY:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A fence or query has not yet completed");
		break;
	case VK_TIMEOUT:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A wait operation has not completed in the "
			       "specified time");
		break;
	case VK_EVENT_SET:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "An event is signaled");
		break;
	case VK_EVENT_RESET:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "An event is unsignaled");
		break;
	case VK_INCOMPLETE:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX, "
			       "array was too small for the result");
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A host memory allocation has failed");
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A device memory allocation has failed");
		break;
	case VK_ERROR_INITIALIZATION_FAILED:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "Initialization of an object could not be "
			       "completed for implementation-specific "
			       "reasons");
		break;
	case VK_ERROR_DEVICE_LOST:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "The logical or physical device has been lost");
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "Mapping of a memory object has failed");
		break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A requested layer is not present or could not "
			       "be loaded");
		break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A requested extension is not supported");
		break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A requested feature is not supported");
		break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "The requested version of Vulkan is not "
			       "supported by the driver or is otherwise "
			       "incompatible");
		break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "Too many objects of the type have already "
			       "been created");
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A requested format is not supported on this "
			       "device");
		break;
	case VK_ERROR_SURFACE_LOST_KHR:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A surface is no longer available");
		break;
	case VK_SUBOPTIMAL_KHR:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A swapchain no longer matches the surface "
			       "properties exactly, but can still be used");
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A surface has changed in such a way that it "
			       "is no longer compatible with the swapchain");
		break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "The display used by a swapchain does not use "
			       "the same presentable image layout");
		break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "The requested window is already connected to "
			       "a VkSurfaceKHR, or to some other non-Vulkan "
			       "API");
		break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX,
			       "A validation layer found an error");
		break;
	default:
		stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX, "Unknown error");
	}

	stbsp_snprintf(buf + strlen(buf), PURPL_STATIC_BUF_MAX,
		       " (VkResult %d)", result);

	return buf;
}
