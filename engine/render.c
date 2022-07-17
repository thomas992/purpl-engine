// Rendering abstraction

#include "engine.h"
#include "render.h"

const char *engine_render_api_name(render_api_t api)
{
	switch (api) {
	case RENDER_API_VULKAN:
		return "Vulkan";
	case RENDER_API_DIRECTX:
		return "Direct3D 12";
	case RENDER_API_METAL:
		return "Metal";
	}

	return "Unknown";
}

bool engine_render_init(render_api_t api)
{
	g_engine->render_api = api;

	switch (g_engine->render_api) {
#ifndef __APPLE__
	case RENDER_API_VULKAN:
		return engine_vulkan_init();
#endif
#ifdef _WIN32
	case RENDER_API_DIRECTX:
		return engine_directx_init();
#endif
#ifdef __APPLE__
	case RENDER_API_METAL:
		return engine_metal_init();
#endif
	default:
		PURPL_LOG(RENDER_LOG_PREFIX "Render initialization attempted with invalid API %d\n", api);
		return false;
	}
}

bool engine_render_begin_frame(uint64_t delta)
{
	switch (g_engine->render_api) {
#ifdef VULKAN_ENABLED
	case RENDER_API_VULKAN:
		return engine_vulkan_begin_frame(delta);
#endif
#ifdef DIRECTX_ENABLED
	case RENDER_API_DIRECTX:
		return engine_directx_begin_frame(delta);
#endif
#ifdef METAL_ENABLED
	case RENDER_API_METAL:
		return engine_metal_begin_frame(delta);
#endif
	default:
		PURPL_LOG(RENDER_LOG_PREFIX "Frame setup attempted with invalid API %d\n", g_engine->render_api);
		return false;
	}
}

bool engine_render_end_frame(uint64_t delta)
{
	switch (g_engine->render_api) {
#ifdef VULKAN_ENABLED
	case RENDER_API_VULKAN:
		return engine_vulkan_end_frame(delta);
#endif
#ifdef DIRECTX_ENABLED
	case RENDER_API_DIRECTX:
		return engine_directx_end_frame(delta);
#endif
#ifdef METAL_ENABLED
	case RENDER_API_METAL:
		return engine_metal_end_frame(delta);
#endif
	default:
		PURPL_LOG(RENDER_LOG_PREFIX "Frame draw attempted with invalid API %d\n", g_engine->render_api);
		return false;
	}
}

void engine_render_shutdown(void)
{
	switch (g_engine->render_api) {
#ifdef VULKAN_ENABLED
	case RENDER_API_VULKAN:
		engine_vulkan_shutdown();
		break;
#endif
#ifdef DIRECTX_ENABLED
	case RENDER_API_DIRECTX:
		engine_directx_shutdown();
		break;
#endif
#ifdef METAL_ENABLED
	case RENDER_API_METAL:
		engine_metal_shutdown();
		break;
#endif
	default:
		PURPL_LOG(RENDER_LOG_PREFIX "Render shutdown attempted with invalid API %d\n", g_engine->render_api);
		break;
	}
}
