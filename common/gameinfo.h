// game.ini representation

#pragma once

#include "common.h"
#include "ini.h"
#include "pack.h"

// Structure representing game.ini
typedef struct gameinfo {
	char *gamedir; // The directory of the file's game
	char *path; // The path to the file

	char *game; // The name of the game
	char *title; // The title of the game
	uint32_t version; // The version of the game

	char **dirs; // Paths to the directories
	uint16_t dir_count; // Number of directories
	pack_file_t **packs; // Pack files
	uint16_t pack_count; // Number of pack files
} gameinfo_t;

// Parse a game.ini file
extern gameinfo_t *gameinfo_parse(const char *name, const char *gamedir);

// Clean up a gameinfo_t's data
extern void gameinfo_free(gameinfo_t *info);
