// Structures containing data for the engine instance related to graphics APIs
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

#ifndef PURPL_BUILD
#error This header should only be included in engine files
#endif // !PURPL_BUILD

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

#define PURPL_ALIAS_GRAPHICS_DATA(api) struct purpl_instance_##api *api = &purpl_inst->graphics.api

#ifdef __APPLE__
/// This is going to be a pointer to a Swift structure
typedef void *purpl_instance_metal_t;
#else // __APPLE__
#include "vulkan/inst_data.h"
#endif // __APPLE__
