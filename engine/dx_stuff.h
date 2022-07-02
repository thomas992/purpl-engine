// Direct3D stuff

#pragma once

#include <d3d12.h>

// Direct3D initialization
extern bool engine_directx_init(void);

// Prepare to draw a frame
extern bool engine_directx_begin_frame(uint64_t delta);

// Finish commands
extern bool engine_directx_end_frame(uint64_t delta);

// Shut down Direct3D
extern void engine_directx_shutdown(void);
