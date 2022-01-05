// Initialization functions
//
// Copyright 2021 MobSlicer152
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

PURPL_API bool purpl_init(const char *app_name, enum purpl_graphics_api graphics_api)
{
	purpl_inst = calloc(1, sizeof(struct purpl_instance));
	if (!purpl_inst) {
#ifdef PURPL_DEBUG
		fprintf(stderr,
			"Error: failed to allocate memory for the engine instance: %s\n",
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

	// Initialize libraries
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		PURPL_LOG_CRITICAL(purpl_inst->logger, "Failed to initialize SDL: %s", SDL_GetError());
		purpl_log_close(purpl_inst->logger, true);
		free(purpl_inst->app_name);
		free(purpl_inst);
		return false;
	}

	purpl_inst->wnd = SDL_CreateWindow(app_name, SDL_WINDOWPOS_UNDEFINED,
					   SDL_WINDOWPOS_UNDEFINED, 1024, 768,
					   graphics_api);
	if (!purpl_inst->wnd) {
		PURPL_LOG_CRITICAL(purpl_inst->logger, "Failed to create a window: %s", SDL_GetError());
		purpl_log_close(purpl_inst->logger, true);
		purpl_graphics_destroy_context(purpl_inst->wnd_ctx);
		free(purpl_inst->app_name);
		free(purpl_inst);
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Engine initialization finished at #t"
		       " for application %s", purpl_inst->app_name);

	return true;
}

PURPL_API void purpl_shutdown(void)
{
	PURPL_LOG_WARNING(purpl_inst->logger, "Engine shutting down at #t");
	purpl_log_close(purpl_inst->logger, true);
	purpl_graphics_destroy_context(purpl_inst->wnd_ctx);
	free(purpl_inst->app_name);

	free(purpl_inst);
}
