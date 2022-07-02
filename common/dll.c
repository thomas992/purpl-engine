// DLL loading interface

#include "dll.h"

// DLL file prefix and extension
#define DLL_PREFIX "lib"
#ifdef _WIN32
#define DLL_EXT ".dll"
#elif defined __APPLE__
#define DLL_EXT ".dylib"
#else
#define DLL_EXT ".so"
#endif

// System-specific DLL loading function (calls LoadLibrary or dlopen)
extern bool sys_dll_load(dll_t *dll, bool engine);

// System-specific DLL unloading function (calls FreeLibrary or dlclose)
extern void sys_dll_unload(dll_t *dll);

dll_t *dll_load(const char *path, bool engine)
{
	dll_t *dll;
	char *path2;
	char *tmp;

	dll = util_alloc(1, sizeof(dll_t), NULL);
	path2 = util_normalize_path(path);
	dll->path = util_strdup(path2);

	PURPL_LOG("Searching for DLL matching %s\n", path2);

	if (!util_fexist(dll->path)) {
		tmp = util_append(dll->path, DLL_EXT);
		free(dll->path);
		dll->path = tmp;
	}
	if (!util_fexist(dll->path)) {
		tmp = util_insert(dll->path, strrchr(dll->path, '/') ? strrchr(dll->path, '/') - dll->path : 0,
				  DLL_PREFIX);
		free(dll->path);
		dll->path = tmp;
	}
	if (!util_fexist(dll->path)) {
		PURPL_LOG("DLL matching %s does not exist\n", path2);
		dll_unload(dll);
		return NULL;
	}

	PURPL_LOG("Loading DLL %s\n", dll->path);

	if (!sys_dll_load(dll, engine)) {
		dll_unload(dll);
		return NULL;
	}

	if (engine && dll->version != PURPL_VERSION)
		PURPL_LOG("Version mismatch, DLL v%u.%u.%u.%u, engine v%u.%u.%u.%u. EXPECT CATASROPHIC FAILURE!\n",
			  PURPL_VERSION_FORMAT(dll->version), PURPL_VERSION_FORMAT(PURPL_VERSION));

	return dll;
}

void dll_unload(dll_t *dll)
{
	if (!dll)
		return;

	PURPL_LOG("Unloading DLL %s\n", dll->path);
	free(dll->path);
	sys_dll_unload(dll);
	free(dll);
}
