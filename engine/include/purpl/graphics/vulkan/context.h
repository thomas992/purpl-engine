// Functions for Vulkan-based graphics contexts
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

#include "purpl/graphics/api.h"

#include "misc.h"

/// Converts a version from the PURPL_MAKE_VERSION macro to the format used by
/// Vulkan
#define PURPL_VERSION_TO_VK_VERSION(version)             \
	VK_MAKE_API_VERSION(0, ((version) >> 16) & 0xFF, \
			    ((version) >> 8) & 0xFF, (version)&0xFF)

/// A structure representing the data for a Vulkan context
struct purpl_vulkan_context {
	enum purpl_graphics_api api; // The graphics API for this context (see
				     // graphics/api.h)
	SDL_Surface *surface; // The surface for this context to render to
	VkInstance inst; // The Vulkan instance of this context
};

/// Create a Vulkan context
///
/// \param context The context to set up
///
/// \return Returns true if the context was created
extern bool purpl_vulkan_create_context(struct purpl_vulkan_context *context);

/// Destroy a Vulkan context
///
/// \param context The context to destroy
extern void purpl_vulkan_destroy_context(struct purpl_vulkan_context *context);
