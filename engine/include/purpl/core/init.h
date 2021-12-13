// Initialization functions
//
// Copyright 2021 MobSlicer152
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

#include <thread>

#include "coredefs.h"
#include "inst.h"

namespace purpl::core
{

/// Initialize the engine
extern PURPL_API bool init(void);

/// Callback for the run function. Should return true if the engine should
/// continue running, false otherwise.
using frame_callback = std::function<bool (chrono::milliseconds delta, void *user)>;

/// Run the engine (after creating a window)
extern PURPL_API void run(frame_callback frame, void *user_ptr);

/// Shut down the engine
extern PURPL_API void shutdown(void);

}

