// Rendering abstraction

#pragma once

#include "common/common.h"
#include "common/util.h"

#include "cglm/cglm.h"

// Rendering API
typedef enum render_api {
	RENDER_API_VULKAN, // Vulkan, non-Apple only
	RENDER_API_DIRECTX, // Direct3D 12, Windows-based only (Windows, ReactOS maybe, Xbox)
	RENDER_API_METAL, // Metal, Apple systems only
} render_api_t;

// Vertex, 7 components for position, colour, and alpha
typedef float vertex_t[7];

// Mesh
typedef struct mesh {
	char *name; // Name of the mesh
	vertex_t *verts; // Vertices that form the model
	vec3 origin; // Centre of the model

} mesh_t;

// Initialize rendering
extern bool engine_render_init(render_api_t api);

// Set up a frame to be drawn
extern bool engine_render_begin_frame(void);

// Draws a frame
extern bool engine_render_end_frame(void);

// Shut down rendering
extern void engine_render_shutdown(void);
