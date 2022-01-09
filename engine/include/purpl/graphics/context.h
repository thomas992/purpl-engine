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

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

#include "./vulkan/context.h"

#include "api.h"

#ifndef PURPL_DEFAULT_GRAPHICS_API
#define PURPL_DEFAULT_GRAPHICS_API PURPL_GRAPHICS_API_VULKAN
#endif // !PURPL_DEFAULT_GRAPHICS_API

/// Structure representing a graphics context
union purpl_graphics_context {
	// Needs to be a struct so they don't get merged
	struct {
		enum purpl_graphics_api api; // The graphics API used for this
					     // context's rendering
		SDL_Surface *surface; // The surface for this context to render
				      // to (typically a window)
	};
	struct purpl_vulkan_context vulkan;
};

/// Create a graphics context
///
/// \param api The graphics API to use
/// \param surface The surface to render to
///
/// \return Returns the new context
extern PURPL_API union purpl_graphics_context *
purpl_graphics_create_context(enum purpl_graphics_api api,
			      SDL_Surface *surface);

/// Update a graphics context
///
/// \param context The context to update
///
/// \return Returns whether the update was successful
extern PURPL_API bool
purpl_graphics_update_context(union purpl_graphics_context *context);

/// Destroy a graphics context
///
/// \param context The context to destroy
extern PURPL_API void
purpl_graphics_destroy_context(union purpl_graphics_context *context);
