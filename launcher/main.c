// Main file of the launcher. Loads the shared libraries for the engine.

#include "common/common.h"
#include "common/dll.h"
#include "common/gameinfo.h"
#include "common/util.h"

#include "engine/engine.h"
#include "engine/render.h"

#define LAUNCHER_LOG_PREFIX "LAUNCHER: "

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
	engine_dll_t *engine;
//	dll_t *client;
//	dll_t *server;
	int32_t i;
	char *arg;
	bool error;
	bool devmode;
	char *basedir;
	char *gamedir;
	char *path;
	gameinfo_t *info;
	render_api_t render_api;

	char *dlls[] = { "SDL2", "zstd" };

	basedir = util_absolute_path(argv[0]);
	*(strrchr(basedir, '/') + 1) = 0;
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Base directory is %s\n", basedir);

	error = false;
	gamedir = NULL;
#ifdef __APPLE__
	render_api = RENDER_API_METAL;
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Setting render API to Metal\n");
#else
	render_api = RENDER_API_VULKAN;
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Setting render API to Vulkan\n");
#endif
#ifdef PURPL_DEBUG
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Enabling developer mode\n");
	devmode = true;
#else
	devmode = false;
#endif
	for (i = 0; i < argc; i++) {
		if (argv[i][0] != '-')
			continue;

		arg = argv[i] + 1;
		if (strcmp(arg, "game") == 0) {
			if (i >= argc - 1) {
				PURPL_LOG(LAUNCHER_LOG_PREFIX "-game requires an argument\n");
				error = true;
				break;
			}
			gamedir = util_absolute_path(argv[++i]);
		} else if ((strcmp(arg, "directx") == 0 || strcmp(arg, "direct3d") == 0) && render_api != RENDER_API_DIRECTX) {
#ifdef DIRECTX_ENABLED
			PURPL_LOG(LAUNCHER_LOG_PREFIX "Setting render API to Direct3D 12\n");
			render_api = RENDER_API_DIRECTX;
#else
			PURPL_LOG("Ignoring %s on unsupported platform\n", argv[i]);
#endif
		} else if ((strcmp(arg, "vulkan") == 0) && render_api == RENDER_API_VULKAN) {
#ifdef VULKAN_ENABLED
			PURPL_LOG(LAUNCHER_LOG_PREFIX "Setting render API to Vulkan\n");
			render_api = RENDER_API_VULKAN;
#else
			PURPL_LOG("Ignoring -vulkan on unsupported platform\n");
#endif
		} else if ((strcmp(arg, "dev") == 0 || strcmp(arg, "debug") == 0) && !devmode) {
			PURPL_LOG(LAUNCHER_LOG_PREFIX "Enabling developer mode\n");
			devmode = true;
		} else if ((strcmp(arg, "nodev") == 0 || strcmp(arg, "nodebug")) == 0 && devmode) {
			PURPL_LOG(LAUNCHER_LOG_PREFIX "Disabling developer mode\n");
			devmode = true;
		} else if (strcmp(arg, "help") == 0) {
			printf("\n-- LIST OF AVAILABLE OPTIONS --\n\n"
			       "-game <gamedir>\t\t- Set the game directory\n"
			       "-directx/-direct3d\t- Use Direct3D 12 for rendering\n"
			       "-vulkan\t\t\t- Use Vulkan for rendering\n"
			       "-dev/-debug\t\t- Enable developer mode\n"
			       "-nodev/-nodebug\t\t- Disable developer mode\n"
			       "\nMost/all options print additional information if used incorrectly\n");
			error = true;
			break;
		} else {
			PURPL_LOG(LAUNCHER_LOG_PREFIX "Ignoring unknown argument %s\n", argv[i]);
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
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Game directory \"%s\" does not exist\n", gamedir);
		free(gamedir);
		free(basedir);
		exit(1);
	}
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Game directory is %s\n", gamedir);

	// Load the other libraries needed
	for (i = 0; i < PURPL_ARRSIZE(dlls); i++) {
		path = util_strfmt("%sbin/%s", basedir, dlls[i]);
		dll_load(path, false);
		free(path);
	}

	path = util_append(basedir, "bin/engine");
	engine = (engine_dll_t *)dll_load(path, true);
	free(path);
	if (!engine) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Failed to load engine\n");
		free(gamedir);
		free(basedir);
		exit(1);
	}

	info = gameinfo_parse("game.ini", gamedir);
	if (!info) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Failed to parse game.ini\n");
		dll_unload((dll_t *)engine);
		free(gamedir);
		free(basedir);
		exit(1);
	}

	// Cast the engine's initialization function pointer correctly, must be the same as engine_init in
	// engine/engine.c.
	// clang-format off
	PURPL_RECAST_FUNCPTR(engine->init, bool, const char *basedir, const char *gamedir, gameinfo_t *game,
			     render_api_t render_api, bool devmode)(basedir, gamedir, info, render_api, devmode);

	run((dll_t *[]){
			(dll_t *)engine,
			// client,
			// server,
		}, 1);
	// clang-format on

	engine->shutdown();

	gameinfo_free(info);

	// dll_unload(client);
	// dll_unload(server);
	dll_unload((dll_t *)engine);

	free(basedir);
	free(gamedir);

	return 0;
}
