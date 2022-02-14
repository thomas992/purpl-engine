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
#include <SDL.h>

/// Structure to hold information about the current instance of the engine
struct purpl_instance {
	char *app_name; // The name of the application that initialized the
			// engine
	u32 app_version; // The version of the application that initialized the
			 // engine

	struct purpl_logger *logger; // The logger for this instance
	time_t start_time; // When the engine started

	SDL_Window *wnd; // The main window for the engine
	u32 wnd_width; // The width of the window
	u32 wnd_height; // The height of the window
	u32 wnd_x; // The X position of the window
	u32 wnd_y; // The Y position of the window
};

extern struct purpl_instance *purpl_inst;
#endif
