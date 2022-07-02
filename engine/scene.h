// Scene function. Scenes are the levels/worlds/etc of games

#pragma once

// Thing that exists in a scene
typedef struct scene_thing {
	struct scene_thing *prev; // Previous thing
	struct scene_thing *next; // Next thing
	char *name; // Name of the thing
} scene_thing_t;

// Scene
typedef struct scene {
	char *name; // Name of the scene
	scene_thing_t *things; // Things in the scene
} scene_t;

// Create a scene
extern scene_t *engine_scene_create(const char *name);

// Select a scene
extern scene_t *engine_scene_select(scene_t *scene);

// Destroy a scene
extern void engine_scene_destroy(scene_t *scene);
