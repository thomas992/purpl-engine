// Pack file functions

#include "pack.h"

pack_file_t *pack_create(const char *name, const char *src)
{
	pack_file_t *pack;
	char *name2;
	char *src2;
	char *dir_path;
	DIR *src_dir;
	struct dirent *ent;
	char *buf;

	if (!name || !src || !util_fexist(src))
		return NULL;

	name2 = util_normalize_path(name);
	src2 = util_normalize_path(src);
	PURPL_LOG("Creating pack file %s_*.pak from directory %s\n", name2, src2);

	pack = calloc(1, sizeof(pack_file_t));
	PURPL_ASSERT(pack);
	
	pack->name = name2;
	dir_path = util_append(pack->name, "_dir.pak");
	pack->dir = fopen(dir_path, "wb+");
	PURPL_ASSERT(pack->dir);

	pack->header.signature = PACK_SIGNATURE;
	pack->header.version = PACK_VERSION;
	
	src_dir = opendir(src2);
	while ((ent = readdir(src_dir))) {
		PURPL_LOG("Adding file %s to pack %s\n", ent->d_name, pack->name);

		stbds_arrgrow()
	}
}
