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

#include "purpl/core/init.h"

PURPL_API bool purpl_init(const char *app_name, u32 app_version,
			  enum purpl_graphics_api graphics_api)
{
	char *title;

	purpl_inst = calloc(1, sizeof(struct purpl_instance));
	if (!purpl_inst) {
#ifdef PURPL_DEBUG
		fprintf(stderr,
			"Error: failed to allocate memory for the engine "
			"instance: %s\n",
			purpl_strerror());
#endif // PURPL_DEBUG
		return false;
	}

	purpl_inst->logger = purpl_log_create("purpl-<date>.log",
					      PURPL_LOG_LEVEL_INFO,
					      PURPL_LOG_LEVEL_MAX, NULL);
	if (!purpl_inst->logger) {
#ifdef PURPL_DEBUG
		fprintf(stderr, "Error: failed to create a logger: %s\n",
			purpl_strerror());
#endif // PURPL_DEBUG
		free(purpl_inst);
		return false;
	}

	purpl_inst->start_time = time(NULL);

	purpl_inst->app_name = purpl_strfmt(NULL, "%s", app_name);
	purpl_inst->app_version = app_version;

	// Initialize libraries
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		PURPL_LOG_CRITICAL(purpl_inst->logger,
				   "Failed to initialize SDL: %s",
				   SDL_GetError());
		purpl_log_close(purpl_inst->logger, true);
		free(purpl_inst->app_name);
		free(purpl_inst);
		return false;
	}

	title = purpl_strfmt(NULL, "%s %s", purpl_inst->app_name, purpl_format_version(purpl_inst->app_version));
	purpl_inst->wnd = SDL_CreateWindow(
		title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1024, 768, SDL_WINDOW_RESIZABLE | graphics_api);
	if (!purpl_inst->wnd) {
		PURPL_LOG_CRITICAL(purpl_inst->logger,
				   "Failed to create a window: %s",
				   SDL_GetError());
		purpl_log_close(purpl_inst->logger, true);
		purpl_graphics_destroy_context(purpl_inst->wnd_ctx);
		free(purpl_inst->app_name);
		free(purpl_inst);
	}

	SDL_GetWindowPosition(purpl_inst->wnd, &purpl_inst->wnd_x,
			      &purpl_inst->wnd_y);
	SDL_GetWindowSize(purpl_inst->wnd, &purpl_inst->wnd_width,
			  &purpl_inst->wnd_height);

	purpl_inst->wnd_ctx = purpl_graphics_create_context(
		graphics_api, SDL_GetWindowSurface(purpl_inst->wnd));

	srand((u32)((u64)purpl_inst ^ ((u64)purpl_inst->logger & (u64)title)));

	PURPL_LOG_INFO(purpl_inst->logger,
		       "Engine #V initialized for application #n #v");

	free(title);

	return true;
}

static bool purpl_handle_events(void)
{
	SDL_Event e;

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
					PURPL_LOG_DEBUG(purpl_inst->logger, "Window moved to (%d, %d)", purpl_inst->wnd_x, purpl_inst->wnd_y);
					break;
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					purpl_inst->wnd_width = e.window.data1;
					purpl_inst->wnd_height =
						e.window.data2;
					PURPL_LOG_DEBUG(purpl_inst->logger, "Window resized to %dx%d", purpl_inst->wnd_width, purpl_inst->wnd_height);
					break;
				case SDL_WINDOWEVENT_CLOSE:
					PURPL_LOG_DEBUG(purpl_inst->logger, "Window closed");
					return false;
				}
			}
			break;
		case SDL_QUIT:
			return false;
		}
	}

	return true;
}

PURPL_API void purpl_run(purpl_frame_func frame, void *user_data)
{
	bool running;

	if (!purpl_inst || !frame) {
		if (purpl_inst)
			PURPL_LOG_WARNING(purpl_inst->logger, "No frame "
							      "callback "
							      "given");
		return;
	}

	running = true;
	while (running) {
		running = purpl_handle_events();
		if (!running) {
			PURPL_LOG_WARNING(purpl_inst->logger, "Received quit "
							      "event, exiting "
							      "loop");
			break;
		}

		if (frame)
			running = frame(0, user_data);
	}
}

PURPL_API void purpl_shutdown(void)
{
	PURPL_LOG_WARNING(purpl_inst->logger, "Engine shutting down");
	purpl_graphics_destroy_context(purpl_inst->wnd_ctx);

	SDL_DestroyWindow(purpl_inst->wnd);
	SDL_Quit();

	purpl_log_close(purpl_inst->logger, true);

	free(purpl_inst->app_name);
	free(purpl_inst);
}
