// purpl/graphics/context.h implementations
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

#include "purpl/core/inst.h"

#include "purpl/graphics/context.h"

PURPL_API struct purpl_graphics_context *
purpl_graphics_create_context(enum purpl_graphics_api api,
			      SDL_Surface *surface)
{
	struct purpl_graphics_context *context;

	if (!purpl_inst || !surface) {
		if (purpl_inst)
			PURPL_LOG_WARNING(purpl_inst->logger, "NULL surface "
							      "specified");
		return NULL;
	}

	context = calloc(1, sizeof(struct purpl_graphics_context));
	if (!context) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to allocate memory for a graphics "
				"context: %s",
				purpl_strerror());
		return NULL;
	}

	switch (api) {
	case PURPL_GRAPHICS_API_VULKAN:
		PURPL_LOG_INFO(purpl_inst->logger, "Creating a Vulkan "
						   "context");
		break;
	default:
		PURPL_LOG_WARNING(purpl_inst->logger, "Unimplemented graphics "
						      "API requested");
		errno = ENOTSUP;
		return NULL;
	}
}

PURPL_API void
purpl_graphics_destroy_context(struct purpl_graphics_context *context)
{
	
}
