// dll.h implementation for Windows

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "dll.h"

bool sys_dll_load(dll_t *dll, bool engine)
{
	dll_create_interface_t create_interface;

	dll->handle = LoadLibraryA(dll->path);
	if (!dll->handle) {
		PURPL_LOG("LoadLibraryA(\"%s\") failed: %u\n", dll->path, GetLastError());
		return false;
	}

	if (engine) {
		*(void **)(&create_interface) = (void *)GetProcAddress(dll->handle, "create_interface");
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
		FreeLibrary(dll->handle);
}
