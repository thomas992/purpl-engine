// Pack file. Pretty much a ripoff of Valve's VPK format but kinda different

// All fields are little endian because fuck big endian (most architectures in wide use like x86, ARM, RISC-V are LE
// anyway)

#pragma once

#include "common.h"
#include "util.h"

// Pack signature (1 signifies this is the first format added)
#define PACK_SIGNATURE "PURPL\1"

// Pack version
#define PACK_VERSION 1

// Split data into 69 MB (nice) files to make it easier to update packs
#define PACK_SPLIT_SIZE 72351744

// Pack header (combined with entry_count entries and the path buffer, forms the "directory", because I couldn't be
// bothered to think of a more accurate name)
typedef struct pack_header {
	uint64_t signature; // Must equal PACK_SIGNATURE
	uint8_t version; // Must equal PACK_VERSION
	uint64_t pathbuf_size; // Size of the path buffer
	uint32_t entry_count; // The number of entries
	uint16_t split_count; // The number of split off data files
} pack_header_t;

// Pack entry
typedef struct pack_entry {
	uint64_t hash; // xxHash of the data uncompressed
	uint64_t path_offset; // Offset to the entry's path in the path buffer
	uint32_t offset; // The offset from the start of the first split file
	uint32_t size; // The size of the compressed data in the file
	uint64_t real_size; // The size of the uncompressed data in memory
} pack_entry_t;

// Pack file
typedef struct pack_file {
	char *name; // Path up until _dir.pak or _###.pak
	FILE *dir; // File stream of the directory
	pack_header_t header; // The header
	char *pathbuf; // Path buffer
	pack_entry_t *entries; // The entries
	FILE **split; // File streams of the split files
} pack_file_t;

// Create a pack file
extern pack_file_t *pack_create(const char *name, const char *src);
