// Discord integration stuff
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

#include "discord_game_sdk.h"

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"
#include "purpl/core/types.h"

#include "util.h"

/// Cooldown for Discord callbacks
#define PURPL_DISCORD_API_COOLDOWN 16

/// Cooldown for updating the activity
#define PURPL_DISCORD_ACTIVITY_COOLDOWN 5000

/// Aliases the Discord data in the engine instance
#define PURPL_ALIAS_DISCORD() struct purpl_instance_discord *discord = &purpl_inst->discord

/// Initializes the Discord SDK
///
/// \return Returns true if Discord initialization was successful, false
///	    otherwise
extern bool purpl_discord_init(void);

/// Shuts down the Discord SDK
extern void purpl_discord_shutdown(void);

/// Runs callbacks
///
/// \param delta The number of milliseconds since the last call
/// 
/// \return Returns true if all callbacks ran successfully
extern bool discord_run_callbacks(u32 delta);

/// Update the activity
///
/// \param delta The number of milliseconds since the last call
extern void discord_update_activity(u32 delta);
