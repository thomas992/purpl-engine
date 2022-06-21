// Main file of the launcher. Loads the shared libraries for the engine.

#include "common.h"
#include "dll.h"
#include "util.h"

int32_t main(int32_t argc, char *argv[])
{
	dll_t *engine;

	engine = dll_load("engine");
	dll_unload(engine);
}
