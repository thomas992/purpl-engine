// Main file of the engine DLL

#include "common.h"
#include "dll.h"
#include "pack.h"

bool engine_init(void)
{
	pack_file_t *pack;

	PURPL_LOG("Initializing engine\n");

	pack = pack_create("purpl", "purpl");

	return true;
}

void engine_frame(uint64_t delta)
{
	PURPL_LOG("Frame update (delta %" PRIu64 ")\n", delta);
}

void engine_shutdown(void)
{
	PURPL_LOG("Shutting down\n");
}

PURPL_INTERFACE void create_interface(dll_t *dll)
{
	if (!dll)
		return;

	PURPL_LOG("Creating interface to the engine\n");

	dll->version = 1;
	dll->init = engine_init;
	dll->frame = engine_frame;
	dll->shutdown = engine_shutdown;
}
