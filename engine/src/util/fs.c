// Filesystem functions (mkdir, link, stat)
//
// Copyright 2022 MobSlicer152
// This file is part of Purpl Engine
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef _WIN32
#define PHNT_VERSION PHNT_THRESHOLD
#include <phnt_windows.h>
#include <phnt.h>
#else // _WIN32
#define _GNU_SOURCE

#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#endif // _WIN32

#include <stb_ds.h>

#include "purpl/util/fs.h"

PURPL_API u32 purpl_translate_mode(u32 mode, bool to_native)
{
	u32 out = 0;

	if (to_native) {
#ifdef _WIN32
		if (mode & PURPL_FS_MODE_READ)
			out |= GENERIC_READ;
		if (mode & PURPL_FS_MODE_WRITE)
			out |= GENERIC_WRITE;
		if (mode & PURPL_FS_MODE_EXECUTE)
			out |= GENERIC_EXECUTE;
#else // _WIN32
		if (mode & PURPL_FS_MODE_READ)
			out |= S_IRUSR | S_IRGRP | S_IROTH;
		if (mode & PURPL_FS_MODE_WRITE)
			out |= S_IWUSR | (mode & PURPL_FS_MODE_EVERYONE ? S_IWGRP | S_IWOTH : 0);
		if (mode & PURPL_FS_MODE_EXECUTE)
			out |= S_IXUSR | S_IXGRP | S_IXOTH;
#endif // _WIN32
	} else {
#ifdef _WIN32
#endif // _WIN32
	}

	return out;
}

PURPL_API char *purpl_pathfmt(size_t *size, const char *path,
			      enum purpl_fs_flags flags)
{
	char *path2;

	path2 = purpl_strrplc(path, "\\", "/", size);

	return path2;
}

PURPL_API bool purpl_path_exists(const char *path)
{
	// Mode and directories don't matter, this would occur first
	return !(!fopen(path, "rb") && errno == ENOENT);
}

PURPL_API bool purpl_mkdir(const char *path, enum purpl_fs_flags flags,
			   enum purpl_file_mode mode)
{
	char *path2;
	char **dir_names = NULL;
	size_t i;

	path2 = purpl_pathfmt(NULL, path, flags);
	if (flags & PURPL_FS_MKDIR_RECURSE) {
		for (i = 0; i < (purpl_strcount(path, "/") -
				 purpl_strcount(path, "//")) + 1; i++) {
			char *p = path2;
			size_t j = 0;

			while ((p = strstr(p + j, "/")) && j < i)
				j++;
			stbds_arrput(dir_names, purpl_strfmt(NULL, "%.*s",
						  (s32)(p - path2), path2));
		}
	} else {
		stbds_arrput(dir_names, path2);
	}

	for (i = 0; i < stbds_arrlenu(dir_names); i++) {
#ifdef _WIN32
		// Maybe I'll add code for dealing with security attributes
		// later
		if (!CreateDirectoryA(dir_names[i], NULL)) {
			errno = purpl_win32_error_to_errno(GetLastError());
			stbds_arrfree(dir_names);
			return false;
		}
#else // _WIN32
		int fd = mkdir(dir_names[i], purpl_translate_mode(mode, true));
		if (fd < 0) {
			stbds_arrfree(dir_names);
			return false;
		}
		close(fd);
#endif // _WIN32
	}

	free(path2);
	stbds_arrfree(dir_names);
	return true;
}

PURPL_API char *purpl_path_directory(const char *path, size_t *size)
{
	
}

PURPL_API char *purpl_path_file(const char *path, size_t *size)
{
	
}

PURPL_API void purpl_stat(const char *path, struct purpl_file_info *info)
{
	
}

PURPL_API size_t purpl_get_size(const char *path)
{

}
