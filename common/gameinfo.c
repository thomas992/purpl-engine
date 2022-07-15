// game.ini parsing

#include "gameinfo.h"

// Callback for ini_browse
static bool parse(const char *section, const char *key, const char *value, gameinfo_t *info)
{
	void *tmp;

	if (strcmp(section, "game") == 0) {
		if (strcmp(key, "game") == 0) {
			info->game = util_strdup(value);
			PURPL_LOG(COMMON_LOG_PREFIX "Game info %s is for game %s\n", info->path, info->game);
		} else if (strcmp(key, "title") == 0) {
			info->title = util_strdup(value);
			PURPL_LOG(COMMON_LOG_PREFIX "Title of game %s is %s\n", info->game, info->title);
		} else if (strcmp(key, "version") == 0) {
			info->version = util_parse_version(value);
			PURPL_LOG(COMMON_LOG_PREFIX "Version of game %s is v" PURPL_PRIVER "\n", PURPL_VERSION_FORMAT(info->version));
		} else {
			PURPL_LOG(COMMON_LOG_PREFIX "Ignoring unknown key %s with value %s in section [%s]\n", key,
				  value, section);
		}
	} else if (strcmp(section, "data") == 0) {
		if (strcmp(key, "dir") == 0) {
			info->dirs = util_alloc(++info->dir_count, sizeof(char *), info->dirs);
			info->dirs[info->dir_count - 1] = util_replace(value, ".", info->gamedir);
			PURPL_LOG(COMMON_LOG_PREFIX "Added directory %s to search paths for game %s\n",
				  info->dirs[info->dir_count - 1], info->game);
		} else if (strcmp(key, "pack") == 0) {
			info->packs = util_alloc(++info->pack_count, sizeof(pack_file_t *), info->packs);
			info->packs[info->pack_count - 1] = pack_load(value);
			PURPL_ASSERT(info->packs[info->pack_count - 1]);
			PURPL_LOG(COMMON_LOG_PREFIX "Added pack %s_*.pak to search paths for game %s\n", value,
				  info->game);
		} else {
			PURPL_LOG(COMMON_LOG_PREFIX "Ignoring unknown key %s with value %s in section [%s]\n", key,
				  value, section);
		}
	} else {
		PURPL_LOG(COMMON_LOG_PREFIX "Ignoring unknown key %s with value %s in section [%s]\n", key, value,
			  section);
	}

	return true;
}

gameinfo_t *gameinfo_parse(const char *name, const char *gamedir)
{
	gameinfo_t *info;
	char *name2;

	if (!name || !strlen(name))
		return NULL;

	name2 = util_append(gamedir, name);

	info = util_alloc(1, sizeof(gameinfo_t), NULL);
	info->gamedir = util_strdup(gamedir);
	info->path = util_absolute_path(name2);
	PURPL_LOG(COMMON_LOG_PREFIX "Parsing gameinfo in %s\n", info->path);

	ini_browse((INI_CALLBACK)parse, info, name2);

	PURPL_LOG(COMMON_LOG_PREFIX "Checking required fields");
	PURPL_ASSERT(info->game);
	PURPL_ASSERT(info->title);

	return info;
}

void gameinfo_free(gameinfo_t *info)
{
	uint16_t i;

	if (!info)
		return;

	if (info->gamedir)
		free(info->gamedir);
	if (info->path)
		free(info->path);
	if (info->game)
		free(info->game);
	if (info->title)
		free(info->title);
	if (info->dirs)
		free(info->dirs);

	if (info->packs) {
		for (i = 0; i < info->pack_count; i++)
			pack_close(info->packs[i]);
	}
}
