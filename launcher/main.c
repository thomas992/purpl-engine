// Main file of the launcher. Loads the shared libraries for the engine.

#include "common.h"
#include "dll.h"
#include "util.h"

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

	basedir = util_normalize_path(argv[0]);
	*(strrchr(basedir, '/') + 1) = 0;
	PURPL_LOG("Base directory is %s\n", basedir);

	error = false;
	gamedir = NULL;
	for (i = 0; i < argc; i++) {
		if (argv[i][0] != '-')
			continue;

		arg = argv[i] + 1;
		if (strcmp(arg, "game")) {
			if (i >= argc - 1) {
				PURPL_LOG("-game requires an argument\n");
				error = true;
				break;
			}
			gamedir = util_normalize_path(argv[++i]);
			if (!util_fexist(gamedir)) {
				PURPL_LOG("Game directory \"%s\" does not exist\n", gamedir);
				error = true;
				break;
			}
		}
	}
	if (error) {
		if (gamedir)
			free(gamedir);
		free(basedir);
		exit(1);
	}

	if (!gamedir)
		gamedir = util_strdup("purpl");
	gamedir = util_prepend(gamedir, basedir);
	if (gamedir[strlen(gamedir) - 1] != '/')
		gamedir = util_append(gamedir, "/");
	PURPL_LOG("Game directory is %s\n", gamedir);

	engine = dll_load("engine");
	engine->init(basedir, gamedir);
	engine->shutdown();
	dll_unload(engine);

	free(basedir);
	free(gamedir);
}
