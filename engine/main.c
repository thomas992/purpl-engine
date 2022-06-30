// Main file of the engine DLL

#include "common.h"
#include "dll.h"
#include "gameinfo.h"
#include "pack.h"

static SDL_Window *g_wnd;

bool engine_init(const char *basedir, const char *gamedir, gameinfo_t *game)
{
	PURPL_LOG("Initializing engine\n");

	PURPL_LOG("Initializing SDL\n");
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		PURPL_LOG("Failed to initialize SDL\n");
		return false;
	}

	g_wnd = SDL_CreateWindow(game->title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_ALLOW_HIGHDPI);

	return true;
}

void engine_frame(uint64_t delta)
{
	PURPL_LOG("\rDelta: %" PRIu64 "ms\t", delta);
}

void engine_shutdown(void)
{
	PURPL_LOG("Shutting down\n");
}

PURPL_INTERFACE void create_interface(dll_t *dll)
{
	if (!dll)
		return;

	PURPL_LOG("Creating interface to the engine\n");

	dll->version = PURPL_VERSION;
	dll->init = (dll_init_t)engine_init;
	dll->frame = engine_frame;
	dll->shutdown = engine_shutdown;
}
