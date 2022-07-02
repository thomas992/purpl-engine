// Rendering abstraction

#include "render.h"

render_api_t g_render_api;
scene_t *g_scene;

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
	g_render_api = api;

	switch (g_render_api) {
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
		PURPL_LOG("Render initialization attempted with invalid API %d\n", api);
		return false;
	}
}

bool engine_render_begin_frame(uint64_t delta)
{
	switch (g_render_api) {
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
		PURPL_LOG("Frame setup attempted with invalid API %d\n", g_render_api);
		return false;
	}
}

void engine_render_add_mesh(const char *name, vertex_t *verts, uint32_t vert_count, vec3 origin)
{
	mesh_t *mesh;

	if (!g_scene || !name || !verts || !vert_count || !origin) {
		PURPL_LOG("Not adding mesh with name %s and %u vertices to scene %s\n", name ? name : "<invalid mesh>", vert_count,
			  g_scene ? g_scene->name : "<invalid scene>");
		return;
	}

	mesh = calloc(1, sizeof(mesh_t));
	PURPL_ASSERT(mesh);

	mesh->name = util_strdup(name);
	mesh->verts = calloc(vert_count, sizeof(vertex_t));
	PURPL_ASSERT(mesh->verts);
	memcpy(mesh->verts, verts, vert_count * sizeof(vertex_t));
	mesh->vert_count = vert_count;
	glm_vec3_copy(origin, mesh->origin);
}

void engine_render_add_light() {}

bool engine_render_end_frame(uint64_t delta)
{
	switch (g_render_api) {
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
		PURPL_LOG("Frame draw attempted with invalid API %d\n", g_render_api);
		return false;
	}
}

void engine_render_shutdown(void)
{
	switch (g_render_api) {
#ifdef VULKAN_ENABLED
	case RENDER_API_VULKAN:
		engine_vulkan_shutdown();
#endif
#ifdef DIRECTX_ENABLED
	case RENDER_API_DIRECTX:
		engine_directx_shutdown();
#endif
#ifdef METAL_ENABLED
	case RENDER_API_METAL:
		engine_metal_shutdown();
#endif
	default:
		PURPL_LOG("Render shutdown attempted with invalid API %d\n", g_render_api);
		break;
	}
}
