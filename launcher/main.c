// Main file of the launcher. Loads the shared libraries for the engine.

#include "common.h"
#include "dll.h"
#include "gameinfo.h"
#include "util.h"

// Enter the loop that runs the engine
void run(dll_t **dlls, uint8_t dll_count)
{
	bool running;
	uint64_t now;
	uint64_t last;
	uint64_t delta;
	uint64_t i;

	last = util_getaccuratetime();
	running = true;
	while (running) {
		now = util_getaccuratetime();
		delta = now - last;

		// Update the DLLs
		for (i = 0; i < dll_count; i++) {
			if (dlls[i] && dlls[i]->frame) {
				running = running && dlls[i]->frame(delta);
			}
		}

		last = now;
	}
}

int32_t main(int32_t argc, char *argv[])
{
	dll_t *engine;
	dll_t *client;
	dll_t *server;
	int32_t i;
	char *arg;
	bool error;
	char *basedir;
	char *gamedir;
	char *path;
	gameinfo_t *info;

	char *dlls[] = { "SDL2", "zstd" };

	basedir = util_absolute_path(argv[0]);
	*(strrchr(basedir, '/') + 1) = 0;
	PURPL_LOG("Base directory is %s\n", basedir);

	error = false;
	gamedir = NULL;
	for (i = 0; i < argc; i++) {
		if (argv[i][0] != '-')
			continue;

		arg = argv[i] + 1;
		if (strcmp(arg, "game") == 0) {
			if (i >= argc - 1) {
				PURPL_LOG("-game requires an argument\n");
				error = true;
				break;
			}
			gamedir = util_absolute_path(argv[++i]);
		}
	}

	if (error) {
		if (gamedir)
			free(gamedir);
		free(basedir);
		exit(1);
	}

	if (!gamedir)
		gamedir = util_prepend("purpl", basedir);
	if (gamedir[strlen(gamedir) - 1] != '/')
		gamedir = util_append(gamedir, "/");
	if (!util_fexist(gamedir)) {
		PURPL_LOG("Game directory \"%s\" does not exist\n", gamedir);
		free(gamedir);
		free(basedir);
		exit(1);
	}
	PURPL_LOG("Game directory is %s\n", gamedir);

	// Load the other libraries needed
	for (i = 0; i < PURPL_ARRSIZE(dlls); i++) {
		path = util_strfmt("%sbin/%s", basedir, dlls[i]);
		dll_load(path, false);
		free(path);
	}

	path = util_append(basedir, "bin/engine");
	engine = dll_load(path, true);
	free(path);
	if (!engine) {
		PURPL_LOG("Failed to load engine\n");
		free(gamedir);
		free(basedir);
		exit(1);
	}

	info = gameinfo_parse("game.ini", gamedir);
	if (!info) {
		PURPL_LOG("Failed to parse game.ini\n");
		dll_unload(engine);
		free(gamedir);
		free(basedir);
		exit(1);
	}

	PURPL_RECAST_FUNCPTR(engine->init, bool, const char *basedir, const char *gamedir, gameinfo_t *info)(basedir, gamedir, info);

	run(
		(dll_t *[]){
			engine,
			// client,
			// server,
		},
		1);

	engine->shutdown();
	dll_unload(engine);

	free(basedir);
	free(gamedir);
}
