// Callbacks for bgfx to use
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

#include <bgfx/c99/bgfx.h>

#include "purpl/graphics/private/callbacks.h"

bgfx_callback_interface_t bgfx_callbacks;

static void bgfx_fatal_callback(const char *file, uint16_t line,
				bgfx_fatal_t code, const char *msg)
{
	purpl_log_write(purpl_inst->logger, PURPL_LOG_LEVEL_ERROR, file, line,
			"bgfx_fatal_callback, called by bgfx",
			"bgfx error message: %s", msg);
#ifndef PURPL_DEBUG
	if (code != BGFX_FATAL_DEBUG_CHECK)
		exit(code);
#endif
}
