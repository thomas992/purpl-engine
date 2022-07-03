// dll.h implementation for Linux

#include <dlfcn.h>

#include "common/dll.h"

bool sys_dll_load(dll_t *dll, bool engine)
{
	dll_create_interface_t create_interface;

	dll->handle = dlopen(dll->path, RTLD_LAZY);
	if (!dll->handle) {
		PURPL_LOG("dlopen(\"%s\", RTLD_LAZY) failed: %s\n", dll->path, dlerror());
		return false;
	}

	if (engine) {
		*(void **)(&create_interface) = dlsym(dll->handle, "create_interface");
		if (!create_interface) {
			PURPL_LOG("DLL %s is not an engine DLL\n", dll->path);
			return false;
		}

		create_interface(dll);
	}
	return true;
}

void sys_dll_unload(dll_t *dll)
{
	if (dll && dll->handle)
		dlclose(dll->handle);
}
