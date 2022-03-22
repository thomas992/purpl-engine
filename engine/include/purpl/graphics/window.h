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

/// Gets the window title
///
/// \return Returns the current title of the engine's window. Must be freed.
extern PURPL_API char *purpl_graphics_window_get_title(void);

/// Sets the window title
///
/// \param title The new title for the window. Supports printf format specifiers.
extern PURPL_API void purpl_graphics_window_set_title(const char *title, ...);

