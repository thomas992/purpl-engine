// Pack file functions

#include "pack.h"

pack_file_t *pack_create(const char *name, const char *src)
{
	pack_file_t *pack;
	char *src2; // Valve better give us Source 2 soon
	char *path;

	if (!name || !src || !strlen(src))
		return NULL;

	pack = calloc(1, sizeof(pack_file_t));
	PURPL_ASSERT(pack);

	pack->name = util_normalize_path(name);
	src2 = util_normalize_path(src);
	PURPL_LOG("Creating pack file %s_*.pak from directory %s\n", pack->name, src2);

	path = util_strfmt("%s_dir.pak", pack->name);
	pack->dir = fopen(path, "wb+");
	PURPL_ASSERT(pack->dir);
	free(path);

	// Other fields are zeroed by calloc, so there's no need to set them
	memcpy(pack->header.signature, PACK_SIGNATURE, PACK_SIGNATURE_LENGTH);
	pack->header.version = PACK_VERSION;
	pack_add_dir(pack, src2);
	free(src2);

	// Write the header, it's been filled in by pack_add_dir
	pack_write(pack);

	return pack;
}

pack_file_t *pack_load(const char *name)
{
	pack_file_t *pack;
	char *path;
	uint32_t i;

	if (!name || !strlen(name))
		return NULL;

	pack = calloc(1, sizeof(pack_file_t));
	PURPL_ASSERT(pack);

	pack->name = util_normalize_path(name);
	PURPL_LOG("Reading pack file %s_*.pak\n", pack->name);

	path = util_strfmt("%s_dir.pak", pack->name);
	pack->dir = fopen(path, "rb");
	if (!pack->dir)
		return NULL;
	free(path);

	fread(&pack->header, sizeof(pack_header_t), 1, pack->dir);
	PURPL_ASSERT(memcmp(pack->header.signature, PACK_SIGNATURE, PACK_SIGNATURE_LENGTH) == 0);

	pack->pathbuf = calloc(pack->header.pathbuf_size, sizeof(char));
	PURPL_ASSERT(pack->pathbuf);
	fread(pack->pathbuf, 1, pack->header.pathbuf_size, pack->dir);
	PURPL_LOG("Read %" PRIu64 " byte path buffer\n", pack->header.pathbuf_size);

	pack->entries = calloc(pack->header.entry_count, sizeof(pack_entry_t));
	PURPL_ASSERT(pack->entries);

	for (i = 0; i < pack->header.entry_count; i++)
		fread(pack->entries + i, sizeof(pack_entry_t), 1, pack->dir);
	PURPL_LOG("Read %u entries\n", pack->header.entry_count);

	return pack;
}

void pack_write(pack_file_t *pack)
{
	if (!pack)
		return;

	PURPL_LOG("Writing pack %s_*.pak with %u %s and %zu %s in the path buffer\n", pack->name,
		  pack->header.entry_count, PURPL_PLURALIZE(pack->header.entry_count, "entries", "entry"),
		  pack->header.pathbuf_size, PURPL_PLURALIZE(pack->header.pathbuf_size, "bytes", "byte"));

	fseek(pack->dir, 0, SEEK_SET);
	fwrite(&pack->header, sizeof(pack_header_t), 1, pack->dir);
	fwrite(pack->pathbuf, 1, pack->header.pathbuf_size, pack->dir);
	fwrite(pack->entries, sizeof(pack_entry_t), pack->header.entry_count, pack->dir);

	PURPL_LOG("Flushing file stream\n");
	fflush(pack->dir);
}

void pack_close(pack_file_t *pack)
{
	if (!pack)
		return;

	PURPL_LOG("Closing pack %s_*.pak\n", pack->name);

	free(pack->name);
	free(pack->pathbuf);
	free(pack->entries);
	fclose(pack->dir);
	free(pack);
}

pack_entry_t *pack_get(pack_file_t *pack, const char *path)
{
	uint64_t hash;
	uint32_t i;
	pack_entry_t *entry = NULL;

	if (!pack || !path || !strlen(path))
		return NULL;

	hash = XXH3_64bits(path, strlen(path) + 1);
	for (i = 0; i < pack->header.entry_count; i++) {
		if (hash == pack->entries[i].path_hash) {
			entry = pack->entries + i;
			break;
		}
	}

	return entry;
}

uint8_t *pack_read(pack_file_t *pack, pack_entry_t *entry)
{
	uint8_t *compressed;
	uint8_t *buf;
	uint16_t split_idx;
	char *split_name;
	FILE *split;
	uint64_t hash;

	if (!pack || !entry)
		return NULL;

	compressed = calloc(entry->size, 1);
	PURPL_ASSERT(compressed);
	buf = calloc(entry->real_size, 1);
	PURPL_ASSERT(buf);

	split_idx = (uint16_t)PACK_SPLIT(entry->offset);
	split_name = util_strfmt("%s_%0.5u.pak", pack->name, split_idx);
#ifdef PACK_DEBUG
	PURPL_LOG("Reading file %s (%u %s compressed, %" PRIu64 " %s raw, stored hash 0x%" PRIX64 ", offset 0x%" PRIu64
		  ") from pack split %s\n",
		  PACK_GET_NAME(pack, entry), entry->size, PURPL_PLURALIZE(entry->size, "bytes", "byte"),
		  entry->real_size, PURPL_PLURALIZE(entry->real_size, "bytes", "byte"), entry->hash, entry->offset,
		  split_name);
#endif
	split = fopen(split_name, "rb");
	PURPL_ASSERT(split);

	fseek(split, PACK_SPLIT_OFFSET(entry->offset), SEEK_SET);
	fread(compressed, 1, entry->size, split);
	fclose(split);
	ZSTD_decompress(buf, entry->real_size, compressed, entry->size);
	free(compressed);

	hash = XXH3_64bits(buf, entry->real_size);
	if (hash != entry->hash) {
		PURPL_LOG("Hash 0x%" PRIX64 " does not match expected hash 0x%" PRIX64 "\n", hash, entry->hash);
		free(buf);
		return NULL;
	}

#ifdef PACK_DEBUG
	PURPL_LOG("Read file %s from pack split %s\n", PACK_GET_NAME(pack, entry), split_name);
#endif

	free(split_name);
	return buf;
}

pack_entry_t *pack_add(pack_file_t *pack, const char *path, const char *internal_path)
{
	char *path2;
	void *tmp;
	size_t len;
	size_t entry_idx;
	pack_entry_t entry;
	FILE *src;
	FILE *dst;
	uint8_t *compressed;
	uint16_t split_idx;
	size_t offset;
	size_t remaining;

	if (!pack || !path || !strlen(path))
		return NULL;

	path2 = util_normalize_path(path);
#ifdef PACK_DEBUG
	PURPL_LOG("Adding file %s to pack %s_*.pak as %s\n", path2, pack->name, internal_path);
#endif

	memset(&entry, 0, sizeof(pack_entry_t));
	entry.offset = PACK_OFFSET(pack);

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
	free(path2);
	entry.real_size = util_fsize(src);
	tmp = calloc(entry.real_size, sizeof(uint8_t));
	PURPL_ASSERT(tmp);

	fread(tmp, 1, entry.real_size, src);
	fclose(src);

	entry.path_hash = XXH3_64bits(PACK_GET_NAME(pack, &entry), strlen(PACK_GET_NAME(pack, &entry)) + 1);
	entry.hash = XXH3_64bits(tmp, entry.real_size);
	entry.size = (uint32_t)ZSTD_compressBound(entry.real_size);
	compressed = calloc(entry.size, 1);
	PURPL_ASSERT(compressed);
	entry.size = (uint32_t)ZSTD_compress(compressed, entry.size, tmp, entry.real_size, ZSTD_btultra2);
#ifdef PACK_DEBUG
	PURPL_LOG("Read %" PRIu64 " %s, hash 0x%" PRIX64 "X, compressed size is %u %s\n", entry.real_size,
		  PURPL_PLURALIZE(entry.real_size, "bytes", "byte"), entry.hash, entry.size,
		  PURPL_PLURALIZE(entry.size, "bytes", "byte"));
#endif
	free(tmp);

	// Write the compressed data, but not the header
	offset = entry.offset;
	remaining = entry.size;
	split_idx = (uint16_t)PACK_SPLIT(offset);
	while (remaining > 0) {
		split_idx = (uint16_t)PACK_SPLIT(offset);
		path2 = util_strfmt("%s_%0.5u.pak", pack->name, split_idx);
		len = min(PACK_SPLIT_SIZE - PACK_SPLIT_OFFSET(offset), remaining);
		dst = fopen(path2, "ab+");
		PURPL_ASSERT(dst);
		free(path2);
		fwrite(compressed + (offset - entry.offset), 1, len, dst);
		fclose(dst);
		remaining = len >= remaining ? 0 : remaining - len;
		offset += len;
	}
	free(compressed);

#ifdef PACK_DEBUG
	if (PACK_SPLIT(offset) != split_idx) {
		PURPL_LOG("Wrote %u %s in pack splits %zu-%u\n", entry.size,
			  PURPL_PLURALIZE(entry.size, "bytes", "byte"), PACK_SPLIT(offset), split_idx);
	} else {
		PURPL_LOG("\rWrote %u %s in pack split %u\n", entry.size, PURPL_PLURALIZE(entry.size, "bytes", "byte"),
			  split_idx);
	}
#endif

	pack->entries[entry_idx] = entry;
	pack->header.total_size += entry.size;
	return pack->entries + entry_idx;
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

		path3 = util_strfmt("%s%s%s", path2, path2[strlen(path2) - 1] == '/' ? "" : "/", ent->d_name);
		if (ent->d_type == DT_DIR)
			pack_add_dir(pack, path3);
		else
			pack_add(pack, path3, strchr(path3, '/') + 1);

		free(path3);
	}

	free(path2);
}
