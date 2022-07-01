// Pack file. Pretty much a ripoff of Valve's VPK format but kinda different and also due to laziness you have to
// recreate files when their contents change rather than them being updated, but I don't forsee that being a huge issue

// All fields are little endian because fuck big endian (most architectures in wide use like x86, ARM, RISC-V are LE
// anyway)

#pragma once

#include "common.h"
#include "util.h"

// Pack signature
#define PACK_SIGNATURE "PURPLPAK"

// Pack signature length
#define PACK_SIGNATURE_LENGTH 8

// Pack version
#define PACK_VERSION 1

// Enable logging in pack_add and pack_read
#define PACK_DEBUG 0

// Split data into 69 MB (nice) files to make it easier to update packs
#define PACK_SPLIT_SIZE 72351744

// Get the last entry
#define PACK_LAST_ENTRY(pack) \
	((pack) && (pack)->header.entry_count ? (pack)->entries + (pack)->header.entry_count - 1 : 0)

// Get the offset of a new file's data
#define PACK_OFFSET(pack) (PACK_LAST_ENTRY((pack)) ? PACK_LAST_ENTRY(pack)->offset + PACK_LAST_ENTRY(pack)->size : 0)

// Get the split number of a file given its offset
#define PACK_SPLIT(offset) ((uint16_t)fmin((double)(offset), (double)(offset) / PACK_SPLIT_SIZE))

// Make an offset relative to a split
#define PACK_SPLIT_OFFSET(offset) ((offset) % PACK_SPLIT_SIZE)

// Get the name of an entry
#define PACK_GET_NAME(pack, entry) \
	((pack) && (entry)->path_offset < (pack)->header.pathbuf_size ? (pack)->pathbuf + (entry)->path_offset : "")

// Pack header (combined with entry_count entries and the path buffer, forms the "directory", because I couldn't be
// bothered to think of a more accurate name)
typedef struct pack_header {
	char signature[8]; // Must equal PACK_SIGNATURE
	uint8_t version; // Must equal PACK_VERSION
	uint64_t pathbuf_size; // Size of the path buffer
	uint32_t entry_count; // The number of entries
	uint64_t total_size; // The total size of all the split files combined
} pack_header_t;

// Pack entry
typedef struct pack_entry {
	uint64_t path_hash; // xxHash of the path
	uint64_t hash; // xxHash of the data uncompressed
	uint64_t path_offset; // Offset to the entry's path in the path buffer
	uint64_t offset; // The offset from the start of the first split file
	uint32_t size; // The size of the compressed data in the file
	uint64_t real_size; // The size of the uncompressed data in memory
} pack_entry_t;

// Pack file
typedef struct pack_file {
	char *name; // Path up until _dir.pak or _#####.pak
	FILE *dir; // File stream of the directory
	pack_header_t header; // The header
	char *pathbuf; // Path buffer
	pack_entry_t *entries; // The entries
} pack_file_t;

// Create a pack file
extern pack_file_t *pack_create(const char *name, const char *src);

// Load a pack file
extern pack_file_t *pack_load(const char *name);

// Write a pack file created by pack_create. Call this before closing if you've added to the pack since it was created.
extern void pack_write(pack_file_t *pack);

// Close a pack file, invalidating all entries
extern void pack_close(pack_file_t *pack);

// Get a file entry from a pack. Do not pass to pack_read, because you'll want the size of the buffer returned.
extern pack_entry_t *pack_get(pack_file_t *pack, const char *path);

// Read a file from a pack
extern uint8_t *pack_read(pack_file_t *pack, pack_entry_t *entry);

// Add a file to a pack file that was opened by pack_create
extern pack_entry_t *pack_add(pack_file_t *pack, const char *path, const char *internal_path);

// Add a directory to a pack file that was opened by pack_create
extern void pack_add_dir(pack_file_t *pack, const char *path);
