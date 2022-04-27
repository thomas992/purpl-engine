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

#include "discord_game_sdk.h"

#include "purpl/util/discord.h"

#include "purpl/core/warnings.h"

#define DISCORD_CHECK_COOL_PEOPLE(discord)            \
	((discord)->user_id == 532320702611587112 ?   \
		 " (my game engine)" :                \
	 ((discord)->user_id == 515919551444025407 || \
	  (discord)->user_id == 448944515294691337 || \
	  (discord)->user_id == 698998936316149780) ? \
		 " (my friend's game engine)" :       \
		       "")

static void discord_activity_callback(void *data, enum EDiscordResult result);
static void discord_log_callback(void *data, enum EDiscordLogLevel level, const char *msg);
static void discord_user_callback(void *data);

bool purpl_discord_init(void)
{
	PURPL_ALIAS_DISCORD();

	struct DiscordCreateParams params = { 0 };
	enum EDiscordResult result;

	if (!purpl_inst)
		return false;

	PURPL_LOG_INFO(purpl_inst->logger, "Initializing Discord");

	discord->user_events.on_current_user_update = discord_user_callback;

	DiscordCreateParamsSetDefault(&params);
	params.client_id = PURPL_DISCORD_APP_ID;
	params.flags = DiscordCreateFlags_NoRequireDiscord;
	params.event_data = NULL;
	params.activity_events = &discord->activity_events;
	params.user_events = &discord->user_events;
	result = DiscordCreate(DISCORD_VERSION, &params, &discord->core);
	if (result != DiscordResult_Ok) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to initialize Discord: DiscordResult %d",
			result);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Setting Discord log callback");
#ifdef PURPL_DEBUG
	discord->core->set_log_hook(discord->core, DiscordLogLevel_Debug, NULL, discord_log_callback);
#else // PURPL_DEBUG
	discord->core->set_log_hook(discord->core, DiscordLogLevel_Warn, NULL, discord_log_callback);
#endif // PURPL_DEBUG

	discord->users = discord->core->get_user_manager(discord->core);
	discord->activities =
		discord->core->get_activity_manager(discord->core);
	discord->application =
		discord->core->get_application_manager(discord->core);

	PURPL_LOG_INFO(purpl_inst->logger, "Successfully initialized Discord");

	return true;
}

void purpl_discord_shutdown(void)
{
	PURPL_ALIAS_DISCORD();

	if (discord->core) {
		PURPL_LOG_WARNING(purpl_inst->logger, "Shutting down Discord");
		discord->core->destroy(discord->core);
	}
}

bool discord_run_callbacks(u32 delta)
{
	PURPL_ALIAS_DISCORD();

	enum EDiscordResult result = DiscordResult_Ok;

	if (!discord->core)
		return false;

	discord->request_cooldown -= delta;
	if (discord->request_cooldown <= 0) {
		result = discord->core->run_callbacks(discord->core);
		discord->request_cooldown =
			PURPL_DISCORD_API_COOLDOWN; // Discord requires this
						    // cooldown
	}

	if (result != DiscordResult_Ok) {
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Discord callbacks failed: DiscordResult %d",
			       result);
		return false;
	}

	return true;
}

/// Result for discord_update_activity
enum EDiscordResult activity_result;

void discord_update_activity(u32 delta)
{
	PURPL_ALIAS_DISCORD();

	struct DiscordActivity activity = { 0 };

	if (!discord->activities)
		return;

	discord->activity_cooldown -= delta;
	if (discord->activity_cooldown > 0)
		return;

	snprintf(activity.name, PURPL_SIZEOF_ARRAY(activity.name), "%s v%s",
		 purpl_inst->app_name,
		 purpl_format_version(purpl_inst->app_version));
	snprintf(activity.details, PURPL_SIZEOF_ARRAY(activity.details),
		 "Purpl Engine%s v%s+%.7s-%.7s-%s",
		 DISCORD_CHECK_COOL_PEOPLE(discord),
		 purpl_format_version(PURPL_VERSION), PURPL_SOURCE_BRANCH,
		 PURPL_SOURCE_COMMIT, PURPL_BUILD_TYPE);
	snprintf(activity.state, PURPL_SIZEOF_ARRAY(activity.state),
		 "%dx%d %s window at (%d, %d)", purpl_inst->wnd_width,
		 purpl_inst->wnd_height,
		 purpl_get_graphics_api_name(purpl_inst->graphics_api),
		 purpl_inst->wnd_x, purpl_inst->wnd_y);

	PURPL_LOG_INFO(purpl_inst->logger, "Updating Discord activity");
	PURPL_LOG_INFO(purpl_inst->logger, "\tName: %s", activity.name);
	PURPL_LOG_INFO(purpl_inst->logger, "\tDetails: %s", activity.details);
	PURPL_LOG_INFO(purpl_inst->logger, "\tState: %s", activity.state);

	discord->activities->update_activity(discord->activities, &activity,
					     NULL, discord_activity_callback);
	discord->activity_cooldown = PURPL_DISCORD_ACTIVITY_COOLDOWN;
}

static void discord_activity_callback(void *data, enum EDiscordResult result)
{
	PURPL_IGNORE(data);

	activity_result = result;
	if (activity_result != DiscordResult_Ok)
		PURPL_LOG_INFO(purpl_inst->logger,
			       "Failed to update activity: DiscordResult %d",
			       activity_result);
}

static void discord_log_callback(void *data, enum EDiscordLogLevel level, const char *msg)
{
	enum purpl_log_level level2;

	PURPL_IGNORE(data);

	switch (level) {
	case DiscordLogLevel_Error:
		level2 = PURPL_LOG_LEVEL_ERROR;
		break;
	case DiscordLogLevel_Warn:
		level2 = PURPL_LOG_LEVEL_WARNING;
		break;
	case DiscordLogLevel_Info:
		level2 = PURPL_LOG_LEVEL_INFO;
		break;
	default:
	case DiscordLogLevel_Debug:
		level2 = PURPL_LOG_LEVEL_DEBUG;
		break;
	}

	purpl_log_write(purpl_inst->logger, level2, __FILE__, __LINE__, PURPL_CURRENT_FUNCTION, "Discord message: %s", msg);
}

static void discord_user_callback(void *data)
{
	PURPL_IGNORE(data);

	PURPL_ALIAS_DISCORD();

	struct DiscordUser user;

	discord->users->get_current_user(discord->users, &user);
	discord->user_id = user.id;

	PURPL_LOG_INFO(purpl_inst->logger, "Discord user updated:");
	PURPL_LOG_INFO(purpl_inst->logger, "\tUsername: %s##%.4s",
		       user.username, user.discriminator);
	PURPL_LOG_INFO(purpl_inst->logger, "\tID: %" PRId64, user.id);
	PURPL_LOG_INFO(purpl_inst->logger, "\tBot: %s",
		       user.bot ? "yes" : "no");
}
