// Rendering abstraction

#include "render.h"

bool engine_render_init(render_api_t api)
{
	return true;
}

bool engine_render_begin_frame(void)
{
	return true;
}

bool engine_render_end_frame(void)
{
	return true;
}

void engine_render_shutdown(void) {}
