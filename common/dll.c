// DLL loading interface

#include "dll.h"

// DLL file prefix and extension
#ifdef _WIN32
#define DLL_EXT ".dll"
#define DLL_PREFIX ""
#elif defined __APPLE__
#define DLL_EXT ".dylib"
#define DLL_PREFIX "lib"
#else
#define DLL_EXT ".so"
#define DLL_PREFIX "lib"
#endif

// System-specific DLL loading function (calls LoadLibrary or dlopen)
extern bool sys_dll_load(dll_t *dll);

// System-specific DLL unloading function (calls FreeLibrary or dlclose)
extern void sys_dll_unload(dll_t *dll);

dll_t *dll_load(const char *path)
{
	dll_t *dll;
	char *path2;

	path2 = util_normalize_path(path);

	dll = calloc(1, sizeof(dll_t));
	PURPL_ASSERT(dll);
	dll->path = path2;

	if (!util_fexist(dll->path)) {
		dll->path = util_prepend(dll->path, DLL_PREFIX);
		dll->path = util_append(dll->path, DLL_EXT);
	}

	PURPL_LOG("Loading DLL %s\n", dll->path);

	if (!sys_dll_load(dll))
		return NULL;

	return dll;
}

void dll_unload(dll_t *dll)
{
	if (!dll)
		return;

	PURPL_LOG("Unloading DLL %s\n", dll->path);
	stbds_arrfree(dll->path);
	sys_dll_unload(dll);
	free(dll);
}
