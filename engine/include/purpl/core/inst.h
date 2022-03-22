// Definition of the engine instance structure
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

#include <time.h>

#include "coredefs.h"
#include "log.h"
#include "types.h"

#ifdef PURPL_BUILD
#include "SDL.h"

#ifndef __APPLE__
#include <vulkan/vulkan.h>
#endif // !__APPLE__

/// Possible graphics APIs (only for shutdown)
enum purpl_graphics_api {
	PURPL_GRAPHICS_API_SOFTWARE = 0, // Software rendering (unimplemented)
	PURPL_GRAPHICS_API_OPENGL = 1, // OpenGL (unimplemented)
#ifndef __APPLE__
	PURPL_GRAPHICS_API_VULKAN = 2, // Vulkan
#endif // !__APPLE__
#ifdef _WIN32
	PURPL_GRAPHICS_API_DIRECT3D = 3, // Direct3D (unimplemented)
#endif // _WIN32
#ifdef __APPLE__
	PURPL_GRAPHICS_API_METAL = 4, // Metal (unimplemented)
#endif // __APPLE__
};

/// Structure to hold information about the current instance of the engine
struct purpl_instance {
	char *app_name; // The name of the application that initialized the
			// engine
	u32 app_version; // The version of the application that initialized the
			 // engine

	struct purpl_logger *logger; // The logger for this instance
	time_t start_time; // When the engine started

	SDL_Window *wnd; // The main window for the engine
	s32 wnd_width; // The width of the window
	s32 wnd_height; // The height of the window
	s32 wnd_x; // The X position of the window
	s32 wnd_y; // The Y position of the window

	union {
#ifdef __APPLE__
		// No Metal support yet
#endif // __APPLE__
#ifndef __APPLE__
		// Vulkan supports everything except macOS (MoltenVK exists,
		// but I'd rather just write it in Metal eventually)
		// Vulkan information
		struct purpl_instance_vulkan {
			VkInstance inst; // Vulkan instance
			VkDebugUtilsMessengerEXT debug_messenger; // Debug messenger

#ifdef GLAD_VULKAN_H_
			GLADapiproc (*vk_get_instance_proc_addr)(
				VkInstance inst, const char *name);
#else // GLAD_VULKAN_H_
			void *(*vk_get_instance_proc_addr)(
				VkInstance inst, const char *name);
#endif // GLAD_VULKAN_H_

			VkPhysicalDevice phys_device; // Physical device
		} vulkan;
#endif // !__APPLE__

		// Must match the largest size the structures can be (most likely this will
		// be Windows eventually, because it supports 3 APIs)
		void *padding[4];
	} graphics;
	enum purpl_graphics_api graphics_api; // The API that was initialized on startup
};

extern struct purpl_instance *purpl_inst;
#endif

