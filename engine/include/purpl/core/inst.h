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

#ifdef PURPL_BUILD
#include <time.h>

#include "SDL.h"

#ifdef PURPL_ENABLE_DISCORD
#include "discord_game_sdk.h"
#endif // PURPL_ENABLE_DISCORD

#include "coredefs.h"
#include "types.h"

#include "purpl/graphics/inst_data.h"

#include "purpl/util/parallel.h"

// Forward declaration of purpl_logger instead of including log.h to reduce chances of rebuild
struct purpl_logger;

/// Initial window width
#define PURPL_INITIAL_WINDOW_WIDTH 1024

/// Initial window height
#define PURPL_INITIAL_WINDOW_HEIGHT 768

/// Possible graphics APIs
enum purpl_graphics_api {
	PURPL_GRAPHICS_API_SOFTWARE = 0, /// Software rendering (unimplemented)
	PURPL_GRAPHICS_API_OPENGL = 1, /// OpenGL (unimplemented)
#ifndef __APPLE__
	PURPL_GRAPHICS_API_VULKAN = 2, /// Vulkan
#endif // !__APPLE__
#ifdef _WIN32
	PURPL_GRAPHICS_API_DIRECT3D = 3, /// Direct3D (unimplemented)
#endif // _WIN32
#ifdef __APPLE__
	PURPL_GRAPHICS_API_METAL = 4, /// Metal
#endif // __APPLE__
	PURPL_GRAPHICS_API_MAX = 5 /// Maximum graphics API number
};

/// Structure to hold information about the current instance of the engine
struct purpl_instance {
	char *app_name; /// The name of the application that initialized the
			/// engine
	u32 app_version; /// The version of the application that initialized the
			 /// engine

	char *engine_dir; /// The directory where the engine executable is
			  /// located
	char *engine_data_dir; /// AppData/XDG_DATA_HOME, plus the app name

	struct purpl_logger *logger; /// The logger for this instance
	time_t start_time; /// When the engine started

	purpl_thread_t graphics_thread; /// Graphics thread (does rendering and stuff)
	purpl_mutex_t graphics_mutex; /// Graphics data mutex

	SDL_Window *wnd; /// The main window for the engine
	char *wnd_title; /// The title of the window
	s32 wnd_width; /// The width of the window
	s32 wnd_height; /// The height of the window
	s32 wnd_x; /// The X position of the window
	s32 wnd_y; /// The Y position of the window

	union {
#ifdef __APPLE__
		purpl_instance_metal_t metal; /// Metal data
#else // __APPLE__
		struct purpl_instance_vulkan vulkan; /// Vulkan data
#endif // __APPLE__
	} graphics;
	enum purpl_graphics_api graphics_api; /// The API that was initialized
					      /// on startup

#ifdef PURPL_ENABLE_DISCORD
	/// Discord information, no mutex needed
	struct purpl_instance_discord {
		s32 request_cooldown; /// Keeps track of the 16 millisecond API
				      /// cooldown
		s32 activity_cooldown; /// Keeps track of the 5 second cooldown for
				       /// updating the activity

		struct IDiscordCore *core;
		struct IDiscordUserManager *users;
		struct IDiscordActivityManager *activities;
		struct IDiscordApplicationManager *application;
		DiscordUserId user_id;

		struct IDiscordUserEvents user_events; /// User events
		struct IDiscordActivityEvents activity_events; /// Activity
							       /// events
	} discord;
#endif // PURPL_ENABLE_DISCORD
};

/// Global engine instance
extern struct purpl_instance *purpl_inst;
#endif
