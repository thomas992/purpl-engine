// dll.h implementation for Linux

#include <dlfcn.h>

#include "dll.h"

bool sys_dll_load(dll_t *dll)
{
	dll_create_interface_t create_interface;

	dll->handle = dlopen(dll->path, RTLD_LAZY);
	PURPL_ASSERT(dll->handle);

	*(void **)(&create_interface) = dlsym(dll->handle, "create_interface");
	if (!create_interface) {
		PURPL_LOG("DLL %s is not an engine DLL\n", dll->path);
		return false;
	}

	create_interface(dll);
	return true;
}

void sys_dll_unload(dll_t *dll)
{
	dlclose(dll->handle);
}
