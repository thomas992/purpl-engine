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

PURPL_API bool purpl_graphics_init(void)
{
	bool init = false;

	if (!purpl_inst)
		return false;

	purpl_inst->graphics_api = PURPL_GRAPHICS_API_MAX;

	purpl_inst->graphics_mutex = purpl_mutex_create();
	purpl_inst->wnd_mutex = purpl_mutex_create();

#ifdef __APPLE__
	init = purpl_metal_init();
	if (!init) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to initialize Metal");
	}
#else // __APPLE__
	init = purpl_vulkan_init();
	if (!init) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to initialize Vulkan, unloading Vulkan library");
		SDL_Vulkan_UnloadLibrary();
	}
#endif // __APPLE__

	if (!init) {
		PURPL_LOG_CRITICAL(purpl_inst->logger, "Unable to initialize any graphics API");
		return false;
	}

	SDL_GetWindowPosition(purpl_inst->wnd, &purpl_inst->wnd_x, &purpl_inst->wnd_y);
	SDL_GetWindowSize(purpl_inst->wnd, &purpl_inst->wnd_width, &purpl_inst->wnd_height);

	PURPL_LOG_INFO(purpl_inst->logger, "Successfully initialized %s graphics",
		       purpl_get_graphics_api_name(purpl_inst->graphics_api));

	return true;
}

PURPL_API s32 purpl_graphics_run(void *data)
{
	bool running;
	u64 now;
	u64 last;
	u64 delta;

	bool (*frame)(u64 delta, void *data) = ((void **)data)[0];
	void *user_data = ((void **)data)[1];

	purpl_inst->graphics_alive = true;

	last = SDL_GetTicks64();
	running = true;
	while (running) {
		now = SDL_GetTicks64();
		delta = now - last;

		if (frame) {
			running = frame(delta, user_data);
			if (!running)
				break;
		}

		running = purpl_graphics_update(delta);
		if (!running)
			break;

		// Make sure to finish drawing before breaking the loop
		running = purpl_inst->graphics_alive;

		last = now;
	}

	purpl_inst->graphics_alive = true;

	PURPL_LOG_WARNING(purpl_inst->logger, "Exiting graphics thread");

	return 0;
}

PURPL_API bool purpl_graphics_update(u64 delta)
{
	if (!purpl_inst)
		return false;

	purpl_mutex_lock(purpl_inst->graphics_mutex);

	switch (purpl_inst->graphics_api) {
	case PURPL_GRAPHICS_API_SOFTWARE:
		return false;
	case PURPL_GRAPHICS_API_OPENGL:
		return false;
#ifndef __APPLE__
	case PURPL_GRAPHICS_API_VULKAN:
		return purpl_vulkan_update(delta);
#endif // !__APPLE__
#ifdef _WIN32
	case PURPL_GRAPHICS_API_DIRECT3D:
		return false;
#endif // _WIN32
#ifdef __APPLE__
	case PURPL_GRAPHICS_API_METAL:
		return purpl_metal_update(delta);
#endif // __APPLE__;
	case PURPL_GRAPHICS_API_MAX:
		return false;
	}

	purpl_mutex_unlock(purpl_inst->graphics_mutex);

	return false;
}

PURPL_API void purpl_graphics_shutdown(void)
{
	if (!purpl_inst)
		return;

	PURPL_LOG_INFO(purpl_inst->logger, "Waiting for graphics thread to finish");
	if (purpl_inst->graphics_alive)
		purpl_inst->graphics_alive = false;
	while (!purpl_inst->graphics_alive)
		;

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
	case PURPL_GRAPHICS_API_MAX:
		break;
	}

	if (purpl_inst->wnd_title)
		free(purpl_inst->wnd_title);
	SDL_DestroyWindow(purpl_inst->wnd);

	purpl_mutex_destroy(purpl_inst->wnd_mutex);
	purpl_mutex_destroy(purpl_inst->graphics_mutex);
}

