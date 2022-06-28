// Main file of the engine DLL

#include "common.h"
#include "dll.h"
#include "pack.h"

bool engine_init(const char *basedir, const char *gamedir)
{
	PURPL_LOG("Initializing engine\n");

	return true;
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

	dll->version = PURPL_VERSION;
	dll->init = (dll_init_t)engine_init;
	dll->shutdown = engine_shutdown;
}
