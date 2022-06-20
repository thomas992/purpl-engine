// Implementations of pack.h

#include "pack.h"

#define PKG_TO_NATIVE16(x) x = SDL_SwapBE16(x)
#define PKG_TO_NATIVE32(x) x = SDL_SwapBE32(x)
#define PKG_HEADER_TO_NATIVE(header)                 \
	{                                            \
		PKG_TO_NATIVE16(header.header_size); \
		PKG_TO_NATIVE16(header.entry_size);  \
		PKG_TO_NATIVE32(header.entry_count); \
		PKG_TO_NATIVE32(header.path_size);   \
	}
#define PKG_ENTRY_TO_NATIVE(entry)                    \
	{                                             \
		PKG_TO_NATIVE32(entry.hash);          \
		PKG_TO_NATIVE32(entry.nameofs_flags); \
		PKG_TO_NATIVE32(entry.offset);        \
		PKG_TO_NATIVE32(entry.size);          \
		PKG_TO_NATIVE32(entry.data_size);     \
	}

pkg_file_t *pkg_parse(const char *path)
{
	pkg_file_t *file;
	uint32_t i;

	file = calloc(1, sizeof(pkg_file_t));
	PURPL_ASSERT(file);
	file->path = util_normalize_path(path);

	PURPL_LOG("Reading package file %s\n", file->path);

	file->fp = fopen(file->path, "rb+");
	PURPL_ASSERT(file->fp);

	PURPL_LOG("\tReading header\n");
	fread(&file->header, sizeof(pkg_header_t), 1, file->fp);
	PKG_HEADER_TO_NATIVE(file->header);
	PURPL_LOG("\tHeader size %u, entry size %u, %u entries\n", file->header.header_size, file->header.entry_size,
		file->header.entry_count);
	PURPL_ASSERT(memcmp(file->header.magic, PKG_MAGIC, 4) == 0);
	PURPL_ASSERT(file->header.header_size == sizeof(pkg_header_t));
	PURPL_ASSERT(file->header.entry_size == sizeof(pkg_entry_t));

	PURPL_LOG("\tReading index (%u entries)\n", file->header.entry_count);
	stbds_arrsetlen(file->entries, file->header.entry_count);
	fread(file->entries, sizeof(pkg_entry_t), file->header.entry_count, file->fp);
	for (i = 0; i < file->header.entry_count; i++)
		PKG_ENTRY_TO_NATIVE(file->entries[i]);

	PURPL_LOG("\tReading path buffer (%u bytes)\n", file->header.path_size);
	stbds_arrsetlen(file->path_buf, file->header.path_size);
	fread(file->path_buf, 1, file->header.path_size, file->fp);

	PURPL_LOG("Done reading package %s\n", file->path);

	return file;
}

void pkg_close(pkg_file_t *file)
{
	if (!file)
		return;

	PURPL_LOG("Closing package file %s\n", file->path);

	stbds_arrfree(file->entries);
	stbds_arrfree(file->path_buf);
	fclose(file->fp);
	stbds_arrfree(file->path);
}

uint8_t *pkg_read(pkg_file_t *file, pkg_entry_t *entry)
{
	uint8_t *buf = NULL;

	if (!file || !entry)
		return NULL;

	PURPL_LOG("Reading file %s (%u bytes%s at offset %u) from package %s\n", pkg_get_entry_name(file, entry), entry->size,
		entry->flags & 1 ? " deflated" : "", entry->offset, file->path);

	fseek(file->fp, entry->offset, SEEK_SET);
	stbds_arrsetlen(buf, entry->size);
	fread(buf, 1, entry->size, file->fp);

	return buf;
}

pkg_entry_t *pkg_get(pkg_file_t *file, const char *path)
{
	uint32_t hash;
	uint32_t i;

	if (!file || !path)
		return NULL;

	hash = pkg_hash(path);
	PURPL_LOG("Getting file %s (hash %u) from package %s\n", path, hash, file->path);

	for (i = 0; i < file->header.entry_count; i++) {
		if (file->entries[i].hash == hash)
			break;
	}

	PURPL_LOG("Found %s at index %u\n", path, i);

	return file->entries + i;
}
