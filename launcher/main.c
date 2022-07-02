// Main file of the launcher. Loads the shared libraries for the engine.

#include "common/common.h"
#include "common/dll.h"
#include "common/gameinfo.h"
#include "common/util.h"

#include "engine/render.h"

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
		for (i = 0; i < dll_count; i++) {
			if (dlls[i] && dlls[i]->begin_frame)
				running = running && dlls[i]->begin_frame(delta);
		}

		now = util_getaccuratetime();
		delta += now - last;
		for (i = 0; i < dll_count; i++) {
			if (dlls[i] && dlls[i]->end_frame)
				running = running && dlls[i]->end_frame(delta);
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
	render_api_t render_api;

	char *dlls[] = { "SDL2", "zstd" };

	basedir = util_absolute_path(argv[0]);
	*(strrchr(basedir, '/') + 1) = 0;
	PURPL_LOG("Base directory is %s\n", basedir);

#ifdef __APPLE__
	api = RENDER_API_METAL;
#else
	render_api = RENDER_API_VULKAN;
#endif
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
		} else if (strcmp(arg, "directx") == 0) {
#ifdef _WIN32
			PURPL_LOG("Setting render API to DirectX 12\n");
//			api = RENDER_API_DIRECTX; DirectX isn't supported yet
#else
			PURPL_LOG("Ignoring -directx on unsupported platform\n");
#endif
		} else if (strcmp(arg, "vulkan") == 0) {
#ifdef __APPLE__
			PURPL_LOG("Ignoring -vulkan because Apple platforms only support Metal\n");
#else
			PURPL_LOG("Setting render API to Vulkan\n");
			render_api = RENDER_API_VULKAN;
#endif
		} else {
			PURPL_LOG("Ignoring unknown argument %s\n", argv[i]);
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

	// Cast the engine's initialization function pointer correctly, must be the same as engine_init in
	// engine/engine.c.
	// clang-format off
	PURPL_RECAST_FUNCPTR(engine->init, bool, const char *basedir, const char *gamedir, gameinfo_t *game,
			     render_api_t render_api)(basedir, gamedir, info, render_api);

	run((dll_t *[]){
			engine,
			// client,
			// server,
		}, 1);
	// clang-format on

	engine->shutdown();

	gameinfo_free(info);

	dll_unload(client);
	dll_unload(server);
	dll_unload(engine);

	free(basedir);
	free(gamedir);
}
