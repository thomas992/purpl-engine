// Engine interface stuff

#pragma once

#include "common/common.h"
#include "common/dll.h"
#include "common/gameinfo.h"
#include "common/pack.h"

#include "render.h"

#define ENGINE_LOG_PREFIX "ENGINE: "

// Interface to the engine
typedef struct engine_dll {
	// Fields shared with dll_t
	char *path; // Path to the DLL
	void *handle; // Handle to the DLL
	bool engine; // Whether the fields after this one are valid

	uint32_t version; // The DLL's version number
	dll_init_t init; // Called at the start
	dll_frame_t begin_frame; // Called at the start of each frame
	dll_frame_t end_frame; // Called at the end of each frame
	dll_shutdown_t shutdown; // Called at the end

	bool dev; // Whether developer mode is enabled
	gameinfo_t *core; // Engine core data info
	gameinfo_t *game; // Game info of the current game

	SDL_Window *wnd; // Window
	int32_t wnd_width; // Window width
	int32_t wnd_height; // Window height
	bool wnd_visible; // Whether the window is visible

	render_api_t render_api; // Graphics API for rendering
	int32_t device_idx; // Index of the graphics device

	ecs_world_t *world; // ECS world
} engine_dll_t;

// Global engine interface
extern engine_dll_t *g_engine;
