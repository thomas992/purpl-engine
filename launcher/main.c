// Main file of the launcher. Loads the shared libraries for the engine.

#include "common/common.h"
#include "common/dll.h"
#include "common/gameinfo.h"
#include "common/util.h"

#include "engine/engine.h"
#include "engine/render.h"

#define LAUNCHER_LOG_PREFIX "LAUNCHER: "

#ifdef __linux__
#define DLL_SUFFIX "_linux"
#else
#define DLL_SUFFIX ""
#endif

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
	char *bindir;
	char *coredir;
	char *gamedir;
	char *path;
	gameinfo_t *coreinfo;
	gameinfo_t *gameinfo;
	render_api_t render_api;

	char *dlls[] = { "flecs", "SDL2", "zstd" };

	basedir = util_absolute_path(argv[0]);
	if (strstr(basedir, "bin")) {
		*(strstr(basedir, "bin")) = 0;
		bindir = util_append(basedir, "bin/");
	} else {
		strrchr(basedir, '/')[1] = 0;
		bindir = util_append(basedir, "bin/");
		if (!util_fexist(bindir)) {
			free(bindir);
			bindir = util_strdup(basedir);
		}
	}
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Base directory is %s, binary directory is %s\n", basedir, bindir);

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
	for (i = 1; i < argc; i++) {
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
//		} else if ((strcmp(arg, "directx") == 0 || strcmp(arg, "direct3d") == 0) &&
//			   render_api != RENDER_API_DIRECTX) {
//#ifdef DIRECTX_ENABLED
//			PURPL_LOG(LAUNCHER_LOG_PREFIX "Setting render API to Direct3D 12\n");
//			render_api = RENDER_API_DIRECTX;
//#else
//			PURPL_LOG("Ignoring %s on unsupported platform\n", argv[i]);
//#endif
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
			       //"-directx/-direct3d\t- Use Direct3D 12 for rendering\n"
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
		free(bindir);
		free(basedir);
		exit(1);
	}

	coredir = util_prepend("core/", basedir);
	if (!util_fexist(coredir)) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Core data directory \"%s\" does not exist\n", coredir);
		free(gamedir);
		free(coredir);
		free(bindir);
		free(basedir);
		exit(1);
	}
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Core data directory is %s\n", coredir);

	if (!gamedir)
		gamedir = util_prepend("purpl", basedir);
	if (gamedir[strlen(gamedir) - 1] != '/')
		gamedir = util_append(gamedir, "/");
	if (!util_fexist(gamedir)) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Game directory \"%s\" does not exist\n", gamedir);
		free(gamedir);
		free(coredir);
		free(bindir);
		free(basedir);
		exit(1);
	}
	PURPL_LOG(LAUNCHER_LOG_PREFIX "Game directory is %s\n", gamedir);

	// Load the other libraries needed
	for (i = 0; i < PURPL_ARRSIZE(dlls); i++) {
		path = util_strfmt("%s/%s" DLL_SUFFIX, bindir, dlls[i]);
		dll_load(path, false);
		free(path);
	}

	path = util_append(bindir, "engine");
	engine = (engine_dll_t *)dll_load(path, true);
	free(path);
	if (!engine) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Failed to load engine\n");
		free(gamedir);
		free(coredir);
		free(bindir);
		free(basedir);
		exit(1);
	}

	coreinfo = gameinfo_parse("game.ini", coredir);
	if (!coreinfo) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Failed to parse %s/game.ini\n", coredir);
		dll_unload((dll_t *)engine);
		free(gamedir);
		free(coredir);
		free(bindir);
		free(basedir);
		exit(1);
	}

	gameinfo = gameinfo_parse("game.ini", gamedir);
	if (!gameinfo) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Failed to parse %s/game.ini\n", gamedir);
		dll_unload((dll_t *)engine);
		gameinfo_free(coreinfo);
		free(gamedir);
		free(coredir);
		free(bindir);
		free(basedir);
		exit(1);
	}

	// Cast the engine's initialization function pointer correctly, must be the same as engine_init in
	// engine/engine.c.
	// clang-format off
	if (!PURPL_RECAST_FUNCPTR(engine->init, bool, const char *basedir, const char *coredir, const char *gamedir, gameinfo_t *core, gameinfo_t *game,
			     render_api_t render_api, bool devmode)(basedir, coredir, gamedir, coreinfo, gameinfo, render_api, devmode)) {
		PURPL_LOG(LAUNCHER_LOG_PREFIX "Engine initialization failed, exiting\n");
		dll_unload((dll_t *)engine);
		gameinfo_free(coreinfo);
		gameinfo_free(gameinfo);
		free(gamedir);
		free(coredir);
		free(bindir);
		free(basedir);
		exit(1);
	}

	run((dll_t *[]){
			(dll_t *)engine,
			// client,
			// server,
		}, 1);
	// clang-format on

	engine->shutdown();

	gameinfo_free(coreinfo);
	gameinfo_free(gameinfo);

	// dll_unload(client);
	// dll_unload(server);
	dll_unload((dll_t *)engine);

	free(basedir);
	free(coredir);
	free(gamedir);

	return 0;
}
