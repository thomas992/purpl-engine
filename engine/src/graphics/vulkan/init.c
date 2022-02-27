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

/// Loads a Vulkan function for glad
///
/// \param inst The engine's Vulkan instance
/// \param name The name of the function to load
/// 
/// \return Returns the return value of vkGetInstanceProcAddr with the same
///	    parameters
static GLADapiproc vulkan_load_func(VkInstance inst, const char *name);

bool purpl_vulkan_init(void)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;

#ifdef __APPLE__
	return false;
#endif // __APPLE__

	if (!purpl_inst)
		return false;

	*((void **)&vulkan->vk_get_instance_proc_addr) = SDL_Vulkan_GetVkGetInstanceProcAddr();

	vulkan_create_instance();

	// This fails due to the device being NULL, but not for any other reason
	gladLoadVulkanUserPtr(NULL, vulkan_load_func, vulkan->inst);

	purpl_inst->graphics_api = PURPL_GRAPHICS_API_VULKAN;
	return true;
}

void purpl_vulkan_shutdown(void)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;

	vkDestroyInstance(vulkan->inst, NULL);
}

static GLADapiproc vulkan_load_func(VkInstance inst, const char *name)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;
	PURPL_LOG_INFO(purpl_inst->logger, "Loading Vulkan function \"%s\"",
		       name);

	return vulkan->vk_get_instance_proc_addr(inst, name);
}
