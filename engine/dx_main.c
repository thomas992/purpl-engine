// Direct3D rendering initialization

#include "engine.h"
#include "render.h"

bool engine_directx_init(void)
{
	if (g_engine->dev) {
		PURPL_LOG("RENDER: Enabling debug layer\n");
		ID3D12Debug debug;

	}
}

bool engine_directx_begin_frame(uint64_t delta)
{
	return false;
}

bool engine_directx_end_frame(uint64_t delta)
{
	return false;
}

void engine_directx_shutdown(void) {}
