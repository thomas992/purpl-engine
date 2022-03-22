// Initialization functions
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

#include <stdlib.h>

#include "coredefs.h"
#include "inst.h"
#include "log.h"
#include "types.h"

#include "purpl/util/util.h"

/// A user-supplied function to be called each frame
///
/// \param delta The number of milliseconds that have passed since the last
///		 call
/// \param user_data The pointer passed to the user_data parameter of
///		     purpl_run
///
/// \return Should return true if nothing went wrong, false otherwise
typedef bool (*purpl_frame_func)(u32 delta, void *user_data);

/// Make sure the engine is ready to be used. You MUST call this function
/// before calling ANY functions from the engine other than this one, or
/// there is a solid chance your program will crash.
extern void purpl_preinit(void);

/// Initialize the engine
///
/// \param app_name The name of the application (used for the window title,
///		    among other things)
/// \param app_version The version of the application (use PURPL_MAKE_VERSION)
/// \param graphics_api The graphics API to use for rendering (pass
///			PURPL_DEFAULT_GRAPHICS_API to use the default, or the
///			API in the config file once those are implemented)
///
/// \return Returns true if the engine initialized. Unless otherwise stated,
/// 	    this means that any functions not from the headers in purpl/util
///	    will not work.
extern PURPL_API bool purpl_init(const char *app_name, u32 app_version);

/// Enter the engine's main loop
///
/// \param frame This function, if supplied, will be called after event
///		 processing
/// \param user_data This pointer will be passed to frame
extern PURPL_API void purpl_run(purpl_frame_func frame, void *user_data);

/// Shut down the engine
extern PURPL_API void purpl_shutdown(void);
