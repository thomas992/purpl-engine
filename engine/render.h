// Rendering abstraction

#pragma once

#include "common/common.h"
#include "common/util.h"

#include "scene.h"

#ifndef __APPLE__
#define VULKAN_ENABLED 1
#endif
#ifdef _WIN32
#define DIRECTX_ENABLED 1
#endif
#ifdef __APPLE__
#define METAL_ENABLED 1
#endif

// Rendering API
typedef enum render_api {
	RENDER_API_VULKAN = 0, // Vulkan, non-Apple only
	RENDER_API_DIRECTX = 1, // Direct3D 12, Windows-based only (Windows, ReactOS maybe, Xbox)
	RENDER_API_METAL = 2, // Metal, Apple systems only
} render_api_t;

// Vertex, 7 components for position, colour, and alpha
typedef float vertex_t[7];

// Mesh
typedef struct mesh {
	// Shared fields from scene_thing_t
	scene_thing_t *prev; // Previous thing
	scene_thing_t *next; // Next thing
	char *name; // Name of the mesh

	vertex_t *verts; // Vertices that form the mesh
	uint32_t vert_count; // Number of vertices in the mesh
	vec3 origin; // Centre of the mesh
	bool enabled; // Whether the mesh is enabled
} mesh_t;

// Light
typedef struct light {
	// Shared fields from scene_thing_t
	scene_thing_t *prev; // Previous thing
	scene_thing_t *next; // Next thing
	char *name; // Name of the light
	
	vec3 colour; // Colour of the light
	float intensity; // Intensity of the light
	vec3 origin; // Position of the light
	bool enabled; // Whether the light is enabled
} light_t;

// Get the name of a render API
extern const char *engine_render_api_name(render_api_t api);

// Initialize rendering
extern bool engine_render_init(render_api_t api);

// Set up a frame to be drawn
extern bool engine_render_begin_frame(uint64_t delta);

// Add a mesh to the current scene
extern void engine_render_add_mesh(const char *name, vertex_t *verts, uint32_t vert_count, vec3 origin);

// Free a mesh
extern void engine_render_free_mesh(mesh_t *mesh);

// Add a light to the current scene
extern void engine_render_add_light(const char *name, vec3 colour, float intensity, vec3 origin);

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
