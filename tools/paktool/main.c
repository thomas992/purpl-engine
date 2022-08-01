// Pack file manipulation tool

#include "common/common.h"
#include "common/pack.h"

#define PAKTOOL_LOG_PREFIX "PAKTOOL: "

// Mode for the tool
typedef enum paktool_mode {
	LIST, // List files
	READ, // Read a file
	EXTRACT, // Extract all the files
	CREATE, // Create a file
	ADD, // Add a file to an existing pack
} paktool_mode_t;

// Display the help message
void usage(bool help);

int32_t main(int32_t argc, char *argv[])
{
	paktool_mode_t mode;
	pack_file_t *pack;
	char *pack_name;
	char *other; // filename or source directory
	char *tmp;
	size_t i;

	if (argc < 3)
		usage(false);

	mode = 0;
	pack_name = NULL;
	if (argc < 2) {
		usage(false);
	} else if (strcmp(argv[1], "help") == 0) {
		usage(true);
	} else if (strcmp(argv[1], "list") == 0 || strcmp(argv[1], "ls") == 0) {
		if (argc < 2)
			usage(false);
		mode = LIST;
	} else if (strcmp(argv[1], "read") == 0 || strcmp(argv[1], "get") == 0) {
		if (argc < 4)
			usage(false);
		mode = READ;
	} else if (strcmp(argv[1], "extract") == 0) {
		if (argc < 3)
			usage(false);
		mode = EXTRACT;
	} else if (strcmp(argv[1], "create") == 0 || strcmp(argv[1], "make") == 0 || strcmp(argv[1], "new") == 0) {
		if (argc < 4)
			usage(false);
		mode = CREATE;
	} else if (strcmp(argv[1], "add") == 0 || strcmp(argv[1], "append") == 0) {
		if (argc < 4)
			usage(false);
		mode = ADD;
	} else {
		usage(false);
	}

	pack_name = util_normalize_path(argv[2]);
	pack = NULL;
	other = NULL;
	switch (mode) {
	case LIST: {
		pack = pack_load(pack_name);
		if (!pack) {
			PURPL_LOG(PAKTOOL_LOG_PREFIX "Failed to read pack file %s\n", pack_name);
			free(pack_name);
			exit(1);
		}

		for (i = 0; i < pack->header.entry_count; i++) {
			printf("Path: %s (hash 0x%" PRIX64 ", offset 0x%" PRIX64 ")\nHash: 0x%" PRIX64
			       "\nCompressed size: %u\nSize: %zu\nOffset: 0x%" PRIX64 "\n\n",
			       pack->pathbuf + pack->entries[i].path_offset, pack->entries[i].path_hash,
			       pack->entries[i].path_offset, pack->entries[i].hash, pack->entries[i].size,
			       pack->entries[i].real_size, pack->entries[i].offset);
		}
		printf("Total of %zu bytes compressed across %lf split files\n", pack->header.total_size,
		       pack->header.total_size / (double)PACK_SPLIT_SIZE);
		break;
	}
	case READ: {
		pack_entry_t *entry;
		uint8_t *buf;
		char *dst_path;
		FILE *dst;

		other = util_normalize_path(argv[3]);

		pack = pack_load(pack_name);
		if (!pack) {
			PURPL_LOG(PAKTOOL_LOG_PREFIX "Failed to read pack file %s\n", pack_name);
			free(pack_name);
			free(other);
			exit(1);
		}

		entry = pack_get(pack, other);
		buf = pack_read(pack, entry);
		if (!buf) {
			PURPL_LOG(PAKTOOL_LOG_PREFIX "Failed to read file %s from pack file %s\n", other, pack_name);
			free(pack_name);
			free(other);
			pack_close(pack);
			exit(1);
		}

		dst_path = strrchr(other, '/') ? strrchr(other, '/') + 1 : "";
		PURPL_LOG(PAKTOOL_LOG_PREFIX "Writing contents of %s to %s\n", other, dst_path);
		dst = fopen(dst_path, "wb+");
		PURPL_ASSERT(dst);

		fwrite(buf, 1, entry->real_size, dst);

		free(buf);
		fclose(dst);
		break;
	}
	case EXTRACT: {
		pack_entry_t *entry;
		uint8_t *buf;
		char *dst_path;
		FILE *dst;
		size_t j;

		if (util_fexist(pack_name)) {
			PURPL_LOG(PAKTOOL_LOG_PREFIX "Removing old directory %s\n", pack_name);
			remove(pack_name);
		}

		pack = pack_load(pack_name);
		if (!pack) {
			PURPL_LOG(PAKTOOL_LOG_PREFIX "Failed to read pack file %s\n", pack_name);
			free(pack_name);
			free(other);
			exit(1);
		}

		PURPL_LOG(PAKTOOL_LOG_PREFIX "Extracting pack file %s\n", pack_name);
		util_mkdir(pack_name);

		for (j = 0; j < pack->header.entry_count; j++) {
			entry = pack->entries + j;

			dst_path = util_strfmt("%s/%s", pack_name, PACK_GET_NAME(pack, entry));
			*strrchr(dst_path, '/') = 0;
			util_mkdir(dst_path);
			dst_path[strlen(dst_path)] = '/';

			buf = pack_read(pack, entry);
			if (!buf) {
				PURPL_LOG(PAKTOOL_LOG_PREFIX "Failed to read file %s from pack file %s\n", other,
					  pack_name);
				free(dst_path);
				free(pack_name);
				free(other);
				pack_close(pack);
				exit(1);
			}

			PURPL_LOG(PAKTOOL_LOG_PREFIX "Extracting %s from %s\n", dst_path, pack_name);
			dst = fopen(dst_path, "wb+");
			PURPL_ASSERT(dst);

			fwrite(buf, 1, entry->real_size, dst);

			free(dst_path);
			free(buf);
			fclose(dst);
		}

		break;
	}
	case CREATE: {
		other = util_normalize_path(argv[3]);

		tmp = util_append(pack_name, "_dir.pak");
		if (util_fexist(tmp)) {
			pack = pack_load(pack_name);
			PURPL_LOG(PAKTOOL_LOG_PREFIX "Removing old files\n");
			remove(tmp);
			free(tmp);
			for (i = 0; i < pack->header.total_size / PACK_SPLIT_SIZE + 1; i++) {
				tmp = util_strfmt("%s_%0.5u.pak", pack_name, i);
				if (util_fexist(tmp))
					remove(tmp);
				free(tmp);
			}
			pack_close(pack);
		} else {
			free(tmp);
		}

		pack = pack_create(pack_name, other);
		if (!pack) {
			PURPL_LOG(PAKTOOL_LOG_PREFIX "Failed to create pack file %s from directory %s\n", pack_name,
				  other);
			free(pack_name);
			free(other);
			exit(1);
		}

		break;
	}
	case ADD: {
		struct stat sb;

		other = util_normalize_path(argv[3]);

		pack = pack_load(pack_name);
		stat(other, &sb);
		if (S_ISDIR(sb.st_mode))
			pack_add_dir(pack, other);
		else
			pack_add(pack, other, other);

		pack_write(pack);

		break;
	}
	}

	pack_close(pack);
	free(pack_name);
	if (other)
		free(other);

	return 0;
}

void usage(bool help)
{
	printf("paktool usage:\n"
	       "\thelp\t\t\t\t\t\t- Print this message\n"
	       "\tlist <pack file>\t\t\t\t- List the files in <pack file>\n"
	       "\tread <pack file> <filename>\t\t\t- Extract <filename> from <pack file>\n"
	       "\textract <pack file>\t\t\t\t- Extract <pack file>\n"
	       "\tcreate <pack file> <source directory>\t\t- Create <pack file> from <source directory>\n"
	       "\tadd <pack file> <source directory or file>\t- Add <source directory or file> to <pack file>\n"
	       "\nNOTE: pack file names should not include the number or _dir or .pak, just the name that comes before\n");
	exit(!help); // Error if help was not requested
}
