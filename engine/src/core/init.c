// Initialization functions
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
#include "SDL_syswm.h"

#include "purpl/core/init.h"

#include "purpl/graphics/init.h"

#ifdef PURPL_ENABLE_DISCORD
#include "purpl/util/discord.h"
#endif // PURPL_ENABLE_DISCORD

#define PREINIT_MAGIC (PURPL_VERSION << 8 | 0x01)

u64 preinit_called;
char *engine_dir;

// Sets preinit_called to PREINIT_MAGIC
PURPL_API s32 purpl_complete_preinit(purpl_main_t main_func, int argc,
				      char *argv[])
{
	preinit_called = PREINIT_MAGIC;

	engine_dir = purpl_pathfmt(NULL, argv[0], 0, false, false);
#ifdef PURPL_WINRT
	PURPL_IGNORE(argc);

	SDL_WinRTRunApp(main_func, NULL);
	return 0;
#else // PURPL_WINRT
	SDL_SetMainReady();
	return main_func(argc, argv);
#endif // PURPL_WINRT
}

PURPL_API void purpl_internal_shutdown(void);

PURPL_API bool purpl_init(const char *app_name, u32 app_version)
{
	char *data_dir;

	purpl_inst = calloc(1, sizeof(struct purpl_instance));
	if (!purpl_inst) {
		fprintf(stderr,
			"Error: failed to allocate memory for the engine instance: %s\n",
			purpl_strerror());
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "Purpl Engine",
			"Failed to allocate the engine instance", NULL);
		return false;
	}

	purpl_inst->engine_dir = purpl_strdup(engine_dir);
	free(engine_dir);

	purpl_inst->app_name =
		purpl_strdup(app_name ? app_name : "purpl-unknown-app");
	purpl_inst->app_version = app_version;

	data_dir = purpl_get_system_data_dir();
	purpl_inst->engine_data_dir =
		purpl_strfmt(NULL, "%s/%s", data_dir, purpl_inst->app_name);
	free(data_dir);
	fprintf(stderr, "Data directory is %s\n", purpl_inst->engine_data_dir);

	if (!purpl_path_exists("logs", false, true))
		purpl_mkdir("logs", PURPL_FS_MKDIR_RECURSE, PURPL_FS_MODE_ALL,
			    false, true);

	purpl_inst->logger = purpl_log_create(NULL, PURPL_LOG_LEVEL_INFO,
					      PURPL_LOG_LEVEL_MAX, NULL);
	if (!purpl_inst->logger) {
		fprintf(stderr, "Error: failed to create a logger: %s\n",
			purpl_strerror());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Purpl Engine",
					 "Failed to create a logger", NULL);
		purpl_internal_shutdown();
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Logger created. Engine "
					   "initialization started.");

	// Check that preinit was called so a warning can be issued otherwise
	PURPL_LOG_DEBUG(purpl_inst->logger,
			"preinit_called = 0x%" PRIX64
			" (PREINIT_MAGIC = 0x%" PRIX64 ")",
			preinit_called, PREINIT_MAGIC);
	if (preinit_called != PREINIT_MAGIC)
		PURPL_LOG_DEBUG(
			purpl_inst->logger,
			"This application has not called purpl_preinit (preinit_called (0x%" PRIX64
			") != PREINIT_MAGIC (0x%" PRIX64 "))",
			preinit_called, PREINIT_MAGIC);
	purpl_inst->start_time = time(NULL);

#ifdef PURPL_ENABLE_DISCORD
	purpl_discord_init();
#endif // PURPL_ENABLE_DISCORD

	PURPL_LOG_INFO(purpl_inst->logger, "Initializing SDL");

	// Initialize libraries
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		PURPL_LOG_CRITICAL(purpl_inst->logger,
				   "Failed to initialize SDL: %s",
				   SDL_GetError());
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "Purpl Engine",
			"Failed to initialize SDL. See the logs for more information.",
			NULL);
		purpl_internal_shutdown();
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Successfully initialized SDL");

	PURPL_LOG_INFO(purpl_inst->logger, "Initializing graphics");
	if (!purpl_graphics_init()) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "Purpl Engine",
			"Failed to initialize graphics. See the logs for more information.",
			NULL);
		purpl_internal_shutdown();
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger,
		       "Purpl Engine #V initialized for application #n #v");

	return true;
}

static bool purpl_handle_events(void)
{
	SDL_Event e;

	// clang-format off
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_WINDOWEVENT:
			if (e.window.windowID ==
			    SDL_GetWindowID(purpl_inst->wnd)) {
				switch (e.window.event) {
				case SDL_WINDOWEVENT_MAXIMIZED:
					PURPL_LOG_DEBUG(purpl_inst->logger, "Window maximized");
					break;
				case SDL_WINDOWEVENT_MOVED:
					purpl_inst->wnd_x = e.window.data1;
					purpl_inst->wnd_y = e.window.data2;
					PURPL_LOG_DEBUG(purpl_inst->logger, "Window moved to (%d, %d)", purpl_inst->wnd_x,
							purpl_inst->wnd_y);
					break;
				case SDL_WINDOWEVENT_RESIZED:
					purpl_inst->wnd_width = e.window.data1;
					purpl_inst->wnd_height =
						e.window.data2;
					PURPL_LOG_DEBUG(purpl_inst->logger, "Window resized to %dx%d", purpl_inst->wnd_width,
						purpl_inst->wnd_height);
					break;
				case SDL_WINDOWEVENT_CLOSE:
					PURPL_LOG_DEBUG(purpl_inst->logger, "Window closed");
					return false;
				}
			}
			break;
		case SDL_QUIT:
			PURPL_LOG_WARNING(purpl_inst->logger,
					  "Received quit event, exiting loop");
			return false;
		}
	}
	// clang-format on

	return true;
}

PURPL_API void purpl_run(purpl_frame_t frame, void *user_data)
{
	bool running;
	u32 now;
	u32 last;
	u32 delta;

	if (!purpl_inst || !frame) {
		if (purpl_inst)
			PURPL_LOG_DEBUG(purpl_inst->logger,
					"No frame callback given");
		return;
	}

	last = SDL_GetTicks();
	running = true;
	while (running) {
		now = SDL_GetTicks();
		delta = now - last;

		running = purpl_handle_events();
		if (!running)
			break;

		if (strcmp(purpl_inst->wnd_title,
			   SDL_GetWindowTitle(purpl_inst->wnd)) != 0) {
			free(purpl_inst->wnd_title);
			purpl_inst->wnd_title = purpl_strdup(
				SDL_GetWindowTitle(purpl_inst->wnd));
		}

#ifdef PURPL_ENABLE_DISCORD
		running = discord_run_callbacks(delta);
		discord_update_activity(delta);
#endif // PURPL_ENABLE_DISCORD

		if (frame)
			running = frame(delta, user_data);
		last = now;
	}
}

PURPL_API void purpl_internal_shutdown(void)
{
	PURPL_LOG_WARNING(purpl_inst->logger, "Engine shutting down");

	purpl_graphics_shutdown();

#ifdef PURPL_ENABLE_DISCORD
	purpl_discord_shutdown();
#endif // PURPL_ENABLE_DISCORD

	SDL_Quit();

	purpl_log_close(purpl_inst->logger, true);

	if (purpl_inst->engine_dir)
		free(purpl_inst->engine_dir);
	if (purpl_inst->engine_data_dir)
		free(purpl_inst->engine_data_dir);

	if (purpl_inst->app_name)
		free(purpl_inst->app_name);
	free(purpl_inst);
}
