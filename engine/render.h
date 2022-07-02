// Rendering abstraction

#pragma once

#include "common/common.h"
#include "common/util.h"

#ifndef __APPLE__
#define VULKAN_ENABLED 1
#endif
#ifdef _WIN32
#define DIRECTX_ENABLED 1
#endif
#ifdef __APPLE__
#define METAL_ENABLED 1
#endif

#define RENDER_LOG_PREFIX ENGINE_LOG_PREFIX "RENDER: "

// Rendering API
typedef enum render_api {
	RENDER_API_VULKAN = 0, // Vulkan, non-Apple only
	RENDER_API_DIRECTX = 1, // Direct3D 12, Windows-based only (Windows, ReactOS maybe, Xbox)
	RENDER_API_METAL = 2, // Metal, Apple systems only
} render_api_t;

// Get the name of a render API
extern const char *engine_render_api_name(render_api_t api);

// Initialize rendering
extern bool engine_render_init(render_api_t api);

// Set up a frame to be drawn
extern bool engine_render_begin_frame(uint64_t delta);

// Draw a frame
extern bool engine_render_end_frame(uint64_t delta);

// Shut down rendering
extern void engine_render_shutdown(void);

// Include the headers for the individual APIs
#ifdef VULKAN_ENABLED
#include "vk_glad.h"
#include "vk_stuff.h"
#endif
#ifdef DIRECTX_ENABLED
#include "dx_stuff.h"
#endif
