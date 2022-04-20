// Graphics initialization/shutdown functions
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

#include "purpl/graphics/init.h"

#include "purpl/core/warnings.h"

PURPL_API bool purpl_graphics_init(void)
{
	bool init = false;

	if (!purpl_inst)
		return false;

#ifdef __APPLE__
	init = purpl_metal_init();
	if (!init) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to initialize Metal");
	}
#else // __APPLE__
	init = purpl_vulkan_init();
	if (!init) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to initialize Vulkan, unloading Vulkan library");
		SDL_Vulkan_UnloadLibrary();
	}
#endif // __APPLE__

	if (!init) {
		PURPL_LOG_CRITICAL(purpl_inst->logger,
				   "Unable to initialize any graphics API");
		return false;
	}

	SDL_GetWindowPosition(purpl_inst->wnd, &purpl_inst->wnd_x,
			      &purpl_inst->wnd_y);
	SDL_GetWindowSize(purpl_inst->wnd, &purpl_inst->wnd_width,
			  &purpl_inst->wnd_height);

	PURPL_LOG_INFO(purpl_inst->logger,
		       "Successfully initialized graphics");

	return true;
}

PURPL_API void purpl_graphics_shutdown(void)
{
	if (!purpl_inst)
		return;

	PURPL_LOG_WARNING(purpl_inst->logger, "Shutting down graphics");

	switch (purpl_inst->graphics_api) {
	case PURPL_GRAPHICS_API_SOFTWARE:
		break;
	case PURPL_GRAPHICS_API_OPENGL:
		break;
#ifndef __APPLE__
	case PURPL_GRAPHICS_API_VULKAN:
		purpl_vulkan_shutdown();
		break;
#endif // !__APPLE__
#ifdef _WIN32
	case PURPL_GRAPHICS_API_DIRECT3D:
		break;
#endif // _WIN32
#ifdef __APPLE__
	case PURPL_GRAPHICS_API_METAL:
		purpl_metal_shutdown();
		break;
#endif // __APPLE__;
	}

	if (purpl_inst->wnd_title)
		free(purpl_inst->wnd_title);
	if (purpl_inst->wnd)
		SDL_DestroyWindow(purpl_inst->wnd);
}
