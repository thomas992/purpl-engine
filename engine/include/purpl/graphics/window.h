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

#pragma once

#include <stdarg.h>
#include <stdlib.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/log.h"

#include "purpl/util/util.h"

#ifdef PURPL_BUILD
#define PURPL_DEFAULT_WINDOW_FLAGS (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)

/// Gets the default window title so that it doesn't need to be in every
/// graphics initialization function
///
/// \reutrn Returns the default window title
extern char *purpl_get_initial_window_title(void);
#endif // PURPL_BUILD

/// Gets the window title
///
/// \return Returns the current title of the engine's window. Must be freed.
extern PURPL_API char *purpl_graphics_window_get_title(void);

/// Sets the window title. DO NOT CALL THIS TOO OFTEN, WINDOWS WILL LAG.
///
/// \param title The new title for the window. Supports printf format
/// specifiers.
extern PURPL_API void purpl_graphics_window_set_title(const char *title, ...);
