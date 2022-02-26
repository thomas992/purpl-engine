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

#include "SDL.h"

#include "purpl/graphics/vulkan/init.h"

bool purpl_vulkan_init(void)
{
#ifdef __APPLE__
	return false;
#endif // __APPLE__

	if (!purpl_inst)
		return false;

	vulkan_create_instance();

	purpl_inst->graphics_api = PURPL_GRAPHICS_API_VULKAN;
	return true;
}

void purpl_vulkan_shutdown(void)
{
	struct purpl_instance_vulkan *vulkan = &purpl_inst->graphics.vulkan;

	vkDestroyInstance(vulkan->inst, NULL);
}
