// Initialization functions
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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "coredefs.h"
#include "inst.h"
#include "types.h"

/// Initialize the engine
///
/// \param app_name The name of the application (used for the window title,
///		    among other things)
/// \param graphics_api The graphics API to use for rendering (pass
///			PURPL_DEFAULT_GRAPHICS_API to use the default, or the
///			API in the config file once those are implemented)
///
/// \return Returns true if the engine initialized. Unless otherwise stated,
/// 	    this means that any functions not from the headers in purpl/util
//	    will not work.
extern PURPL_API bool purpl_init(const char *app_name, enum purpl_graphics_api graphics_api);

/// Shut down the engine
extern PURPL_API void purpl_shutdown(void);
