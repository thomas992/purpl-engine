// DLL loading interface. When a DLL is loaded, it's expected to export a function called create_interface that fills a
// dll_t structure with functions to be called at various points. This mechanism is what Quake and Source engine employ,
// and likely other engines. I chose it because it seems way better and simpler than using three different shitty Python
// scripts to generate an interface header.

#pragma once

#include "common.h"
#include "util.h"

// Interface creation function, expected to set the version and function pointer fields of dll (cast it to or declare
// it as dll_t * instead of void *) but nothing else
typedef void (*dll_create_interface_t)(void *dll);

// Initialization function
typedef bool (*dll_init_t)(const char *basedir, const char *gamedir);

// Frame function
typedef void (*dll_frame_t)(uint64_t delta);

// Shutdown function
typedef void (*dll_shutdown_t)(void);

// A structure representing the interface to a DLL
typedef struct dll {
	char *path; // Path to the DLL
	void *handle; // Handle to the DLL
	uint32_t version; // The DLL's version number
	dll_init_t init; // Called at the start
	dll_frame_t frame; // Called each frame or update
	dll_shutdown_t shutdown; // Called at the end
} dll_t;

// Load a DLL
extern dll_t *dll_load(const char *path);

// Unload a DLL
extern void dll_unload(dll_t *dll);
