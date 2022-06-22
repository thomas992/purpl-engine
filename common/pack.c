// Pack file functions

#include "pack.h"

// Get the last entry
#define PACK_LAST_ENTRY(pack) ((pack) ? (pack)->entries + (pack)->header.entry_count - 1 : 0)

// Get the offset of a file's data
#define PACK_OFFSET(pack) ((pack) ? PACK_LAST_ENTRY(pack)->offset + PACK_LAST_ENTRY(pack)->size : 0)

// Get the split number of a file given its offset
#define PACK_SPLIT(offset) ((offset) / PACK_SPLIT_SIZE)

pack_file_t *pack_create(const char *name, const char *src)
{
	pack_file_t *pack;
	char *src2; // Valve better give us Source 2 soon
	char *dir_path;

	if (!name || !src || !strlen(src))
		return NULL;

	pack = calloc(1, sizeof(pack_file_t));
	PURPL_ASSERT(pack);
	
	pack->name =  util_normalize_path(name);
	src2 = util_normalize_path(src);
	PURPL_LOG("Creating pack file %s_*.pak from directory %s\n", pack->name, src2);

	dir_path = util_strfmt("%s_dir.pak", pack->name);
	pack->dir = fopen(dir_path, "wb+");
	PURPL_ASSERT(pack->dir);
	free(dir_path);

	// Other fields are zeroed by calloc, so there's no need to set them
	memcpy(pack->header.signature, PACK_SIGNATURE, 8);
	pack->header.version = PACK_VERSION;
	pack_add_dir(pack, src2);
	free(src2);

	return pack;
}

void pack_add(pack_file_t *pack, const char *path, const char *internal_path)
{
	char *path2;
	void *tmp;
	size_t len;
	size_t entry_idx;
	pack_entry_t entry;
	FILE *src;
	uint8_t *compressed;
	uint16_t split_idx;

	if (!pack || !path || !strlen(path))
		return;

	path2 = util_normalize_path(path);
	PURPL_LOG("Adding file %s to pack %s as %s\n", path2, pack->name, internal_path);

	if (!pack->entries) {
		pack->entries = calloc(1, sizeof(pack_entry_t));
		PURPL_ASSERT(pack->entries);
		pack->header.entry_count = 1;
		entry_idx = 0;
	} else {
		tmp = calloc(pack->header.entry_count + 1, sizeof(pack_entry_t));
		PURPL_ASSERT(tmp);
		memcpy(tmp, pack->entries, pack->header.entry_count * sizeof(pack_entry_t));
		free(pack->entries);
		pack->entries = tmp;
		entry_idx = pack->header.entry_count;
		pack->header.entry_count++;
	}
	len = strlen(internal_path) + 1;
	if (!pack->pathbuf) {
		pack->pathbuf = calloc(len, sizeof(char));
		PURPL_ASSERT(pack->pathbuf);
		pack->header.pathbuf_size = len;
		strncpy(pack->pathbuf, internal_path, len);
	} else {
		tmp = calloc(pack->header.pathbuf_size + len, sizeof(char));
		PURPL_ASSERT(tmp);
		memcpy(tmp, pack->pathbuf, pack->header.pathbuf_size);
		strncpy((char *)tmp + pack->header.pathbuf_size, internal_path, len);
		free(pack->pathbuf);
		pack->pathbuf = tmp;
		entry.path_offset = pack->header.pathbuf_size;
		pack->header.pathbuf_size += len;
	}

	src = fopen(path2, "rb");
	PURPL_ASSERT(src);
	entry.real_size = util_fsize(src);
	tmp = calloc(entry.real_size, sizeof(uint8_t));
	PURPL_ASSERT(tmp);

	fread(tmp, 1, entry.real_size, src);
	entry.hash = XXH3_64bits(tmp, entry.real_size);

	entry.size = ZSTD_compressBound(entry.real_size);
	compressed = calloc(entry.size, 1);
	PURPL_ASSERT(compressed);
	entry.size = ZSTD_compress(compressed, entry.size, tmp, entry.real_size, ZSTD_btultra2);
	PURPL_LOG("Read %zu bytes, hash 0x%llX, compressed size is %u\n", entry.real_size, entry.hash, entry.size);

	// Write the compressed data, but not the header
	entry.offset = 
	split_idx = PACK_SPLIT(entry.offset);
}

void pack_add_dir(pack_file_t *pack, const char *path)
{
	char *path2;
	char *path3;
	DIR *dir;
	struct dirent *ent;

	if (!pack || !path || !strlen(path))
		return;

	path2 = util_normalize_path(path);
	dir = opendir(path2);
	PURPL_ASSERT(dir);

	while ((ent = readdir(dir))) {
		if (strncmp(ent->d_name, ".", ent->d_namlen) == 0 || strncmp(ent->d_name, "..", ent->d_namlen) == 0)
			continue;

		path3 = util_strfmt("%s/%s", path2, ent->d_name);
		if (ent->d_type == DT_DIR)
			pack_add_dir(pack, path3);
		else
			pack_add(pack, path3, strchr(path3, '/') + 1);

		free(path3);
	}

	free(path2);
}
