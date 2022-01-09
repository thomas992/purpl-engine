// Functions for graphics contexts
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

#include <SDL.h>

/// Graphics APIs for graphics contexts to use
enum purpl_graphics_api {
	PURPL_GRAPHICS_API_SOFTWARE = 0, // Software rendering, will be
					 // implemented eventually
	PURPL_GRAPHICS_API_OPENGL = SDL_WINDOW_OPENGL, // OpenGL, will be worked on
	PURPL_GRAPHICS_API_VULKAN = SDL_WINDOW_VULKAN, // Vulkan, being worked on currently
};
