// Functions for graphics contexts
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

#include "vulkan/context.h"

/// Graphics APIs for graphics contexts to use
enum purpl_graphics_api {
	PURPL_GRAPHICS_API_SOFTWARE = 0, // Software rendering, will be
					 // implemented eventually
	PURPL_GRAPHICS_API_OPENGL, // OpenGL, will be worked on
	PURPL_GRAPHICS_API_VULKAN, // Vulkan, being worked on currently
};

#ifndef PURPL_DEFAULT_GRAPHICS_API
#define PURPL_DEFAULT_GRAPHICS_API PURPL_GRAPHICS_API_VULKAN
#endif // !PURPL_DEFAULT_GRAPHICS_API

/// Structure representing a graphics context
struct purpl_graphics_context {
	SDL_Surface *surf; // The surface for this context to render to
			   // (typically a window)
	enum purpl_graphics_api api; // The graphics API used for this
				     // context's rendering
	void *specific; // API specific data
};

/// Create a graphics context
///
/// \param api The graphics API to use
/// \param surface The surface to render to
///
/// \return Returns the new context
extern PURPL_API struct purpl_graphics_context *
purpl_graphics_create_context(enum purpl_graphics_api api,
			      SDL_Surface *surface);

/// Destroy a graphics context
///
/// \param context The context to destroy
extern PURPL_API void
purpl_graphics_destroy_context(struct purpl_graphics_context *context);
