// Main file of the engine DLL

#include "engine.h"

engine_dll_t *g_engine;

SDL_Window *g_wnd;
int32_t g_wnd_width;
int32_t g_wnd_height;
bool g_wnd_visible;

bool engine_init(const char *basedir, const char *gamedir, gameinfo_t *game, render_api_t render_api, bool devmode)
{
	PURPL_LOG(ENGINE_LOG_PREFIX "Initializing engine for game %s\n", game->title);

	g_engine->dev = devmode;

	PURPL_LOG(ENGINE_LOG_PREFIX "Initializing SDL\n");
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		PURPL_LOG(ENGINE_LOG_PREFIX "Failed to initialize SDL\n");
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
			case SDL_WINDOWEVENT_FOCUS_LOST:
				PURPL_LOG(ENGINE_LOG_PREFIX "Window unfocused\n");
				g_wnd_visible = false;
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				PURPL_LOG(ENGINE_LOG_PREFIX "Window focused\n");
				g_wnd_visible = true;
				break;
			case SDL_WINDOWEVENT_CLOSE:
				PURPL_LOG(ENGINE_LOG_PREFIX "Window closed\n");
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
	PURPL_LOG(ENGINE_LOG_PREFIX "Shutting down\n");

	PURPL_LOG(ENGINE_LOG_PREFIX "Destroying window\n");
	SDL_DestroyWindow(g_wnd);

	PURPL_LOG(ENGINE_LOG_PREFIX "Shutting down SDL\n");
	SDL_Quit();
}

PURPL_INTERFACE void create_interface(engine_dll_t *dll)
{
	if (!dll)
		return;

	PURPL_LOG(ENGINE_LOG_PREFIX "Creating interface to engine v%u.%u.%u.%u\n", PURPL_VERSION_FORMAT(PURPL_VERSION));

	g_engine = dll;
	g_engine->version = PURPL_VERSION;
	g_engine->init = (dll_init_t)engine_init;
	g_engine->begin_frame = engine_begin_frame;
	g_engine->end_frame = engine_end_frame;
	g_engine->shutdown = engine_shutdown;
}
