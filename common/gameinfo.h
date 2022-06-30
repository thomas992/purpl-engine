// game.ini representation

#pragma once

#include "common.h"
#include "ini.h"
#include "pack.h"

// Structure representing game.ini
typedef struct gameinfo {
	char *game; // The name of the game
	char *title; // The title of the game

	char **dirs; // Paths to the directories
	pack_file_t **packs; // Pack files
} gameinfo_t;

// Parse a game.ini file
extern gameinfo_t *gameinfo_parse(const char *path);

// Clean up a gameinfo_t's data
extern void gameinfo_free(gameinfo_t *info);
