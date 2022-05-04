// Functions for controlling the engine window
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

#include "stb_sprintf.h"
#include "SDL.h"

#include "purpl/graphics/window.h"

char *purpl_get_initial_window_title(void)
{
	return purpl_strfmt(NULL, "%s v%s - %s - Purpl Engine v%s+%s-%s-%s", purpl_inst->app_name,
			    purpl_format_version(purpl_inst->app_version),
			    purpl_get_graphics_api_name(purpl_inst->graphics_api), purpl_format_version(PURPL_VERSION),
			    PURPL_SOURCE_BRANCH, PURPL_SOURCE_COMMIT, PURPL_BUILD_TYPE);
}

PURPL_API char *purpl_graphics_window_get_title(void)
{
	return purpl_strdup(purpl_inst->wnd_title);
}

PURPL_API void purpl_graphics_window_set_title(const char *title, ...)
{
	va_list args;
	char *new_title;

	if (!title) {
		PURPL_LOG_DEBUG(purpl_inst->logger, "title was NULL, not setting window title");
		return;
	}

	va_start(args, title);

	new_title = purpl_vstrfmt(NULL, title, args);
	va_end(args);

	// Avoid renaming the window if the strings are the same, it lags
	// Explorer
	if (strcmp(new_title, purpl_inst->wnd_title) == 0) {
		free(new_title);
		return;
	}

	free(purpl_inst->wnd_title);
	purpl_inst->wnd_title = new_title;

	SDL_SetWindowTitle(purpl_inst->wnd, purpl_inst->wnd_title);
}
