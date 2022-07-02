// Main file of the engine DLL

#include "common/common.h"
#include "common/dll.h"
#include "common/gameinfo.h"
#include "common/pack.h"

#include "render.h"

static SDL_Window *g_wnd;
static int32_t g_wnd_width;
static int32_t g_wnd_height;
static bool g_wnd_visible;

bool engine_init(const char *basedir, const char *gamedir, gameinfo_t *game, render_api_t render_api)
{
	PURPL_LOG("Initializing engine for game %s\n", game->title);

	PURPL_LOG("Initializing SDL\n");
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		PURPL_LOG("Failed to initialize SDL\n");
		return false;
	}

	g_wnd_width = 1024;
	g_wnd_height = 576;
	g_wnd = SDL_CreateWindow(game->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, g_wnd_width, g_wnd_height,
				 SDL_WINDOW_ALLOW_HIGHDPI);
	PURPL_ASSERT(g_wnd);

	PURPL_ASSERT(engine_render_init(render_api));

	return true;
}

bool engine_begin_frame(uint64_t delta)
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT && event.window.windowID == SDL_GetWindowID(g_wnd)) {
			switch (event.window.event) {
			case SDL_WINDOWEVENT_HIDDEN:
				PURPL_LOG("Window hidden\n");
				g_wnd_visible = false;
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				PURPL_LOG("Window exposed\n");
				g_wnd_visible = true;
				break;
			case SDL_WINDOWEVENT_CLOSE:
				PURPL_LOG("Window closed\n");
				return false;
			}
		}
	}

	return true;
}

bool engine_end_frame(uint64_t delta)
{
	return true;
}

void engine_shutdown(void)
{
	PURPL_LOG("Shutting down\n");

	PURPL_LOG("Destroying window\n");
	SDL_DestroyWindow(g_wnd);

	PURPL_LOG("Shutting down SDL\n");
	SDL_Quit();
}

PURPL_INTERFACE void create_interface(dll_t *dll)
{
	if (!dll)
		return;

	PURPL_LOG("Creating interface to engine v%u.%u.%u.%u\n", PURPL_VERSION_FORMAT(PURPL_VERSION));

	dll->version = PURPL_VERSION;
	dll->init = (dll_init_t)engine_init;
	dll->begin_frame = engine_begin_frame;
	dll->end_frame = engine_end_frame;
	dll->shutdown = engine_shutdown;
}
