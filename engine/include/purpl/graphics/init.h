// Graphics initialization/shutdown functions
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

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/types.h"

#include "purpl/util/util.h"

#ifndef __APPLE__
#include "vulkan/init.h"
#endif // __APPLE__

/// Initialize graphics for the engine
///
/// \return Returns true if initialization succeeded, false otherwise
extern PURPL_API bool purpl_graphics_init(void);

/// Shut down graphics
extern PURPL_API void purpl_graphics_shutdown(void);
