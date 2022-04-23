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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#else // _WIN32
#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif // _WIN32

#include <stb_ds.h>

#include "purpl/util/fs.h"

#include "purpl/core/warnings.h"

PURPL_API u32 purpl_translate_file_mode(u32 mode, bool to_native)
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
			out |= S_IWUSR | (mode & PURPL_FS_MODE_EVERYONE ?
						  S_IWGRP | S_IWOTH :
							0);
		if (mode & PURPL_FS_MODE_EXECUTE)
			out |= S_IXUSR | S_IXGRP | S_IXOTH;
#endif // _WIN32
	} else {
#ifdef _WIN32
#else // _WIN32
		if (mode & (S_IRUSR | S_IRGRP | S_IROTH))
			out |= PURPL_FS_MODE_READ;
		if (mode & S_IWUSR)
			out |= PURPL_FS_MODE_WRITE;
		if (mode & (S_IWGRP | S_IWOTH))
			out |= PURPL_FS_MODE_EVERYONE;
		if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
			out |= PURPL_FS_MODE_EXECUTE;
#endif // _WIN32
	}

	return out;
}

PURPL_API u32 purpl_translate_file_attrs(u32 attrs, bool to_native)
{
	u32 out = 0;

	PURPL_IGNORE(attrs);

	if (to_native) {
	} else {
#ifdef _WIN32
#else // _WIN32
      // These all overlap and can't be present simultaneously
		if (attrs & S_IFREG)
			out |= PURPL_FILE_ATTR_NORMAL;
		else if (attrs & S_IFSOCK)
			out |= PURPL_FILE_ATTR_DEVICE;
		else if (attrs & S_IFLNK)
			out |= PURPL_FILE_ATTR_SYMLINK;

		if (attrs & S_IFBLK || attrs & S_IFCHR)
			out |= PURPL_FILE_ATTR_DEVICE;
		if (attrs & S_IFIFO)
			out |= PURPL_FILE_ATTR_PIPE;
		if (attrs == (attrs & (S_IRUSR | S_IRGRP | S_IROTH)))
			out |= PURPL_FILE_ATTR_READONLY;
#endif // _WIN32
	}

	return out;
}

PURPL_API char *purpl_pathfmt(size_t *size, const char *path,
			      enum purpl_fs_flags flags, bool relative,
			      bool data_relative)
{
	char *path2;
	char *path3;

	PURPL_IGNORE(flags);

	path2 = purpl_strrplc(path, "\\", "/", size);
	if (path2[0] != '/') {
		if (relative) {
			path3 = purpl_strfmt(NULL, "%s/%s",
					     purpl_inst->engine_dir, path2);
			free(path2);
			path2 = path3;
		} else if (data_relative) {
			path3 = purpl_strfmt(NULL, "%s/%s",
					     purpl_inst->engine_data_dir,
					     path2);
			free(path2);
			path2 = path3;
		}
	}

	return path2;
}

PURPL_API bool purpl_path_exists(const char *path, bool relative,
				 bool data_relative)
{
	FILE *fp;
	char *path2;
	bool ret;

	if (!path)
		return false;

	path2 = purpl_pathfmt(NULL, path, 0, relative, data_relative);
	fp = fopen(path2, "r");
	free(path2);
	if (fp)
		fclose(fp);

	ret = !(!fp && errno == ENOENT);

	return ret;
}

PURPL_API bool purpl_mkdir(const char *path, enum purpl_fs_flags flags,
			   enum purpl_file_mode mode, bool relative,
			   bool data_relative)
{
	char *path2;
	char *path3;
	char **dir_names = NULL;
	size_t i;

	if (!path)
		return false;

	if (mode == 0)
		mode = PURPL_FS_MODE_ALL;

	path2 = purpl_pathfmt(NULL, path, flags, relative, data_relative);
	if (flags & PURPL_FS_MKDIR_RECURSE) {
		stbds_arrsetlen(dir_names, 0);
		for (i = 0; i < strlen(path2); i++) {
			if (path2[i] == '/') {
				path3 = purpl_strndup(path2, i + 1);
				path3[i + 1] = '\0';
				stbds_arrput(dir_names, path3);
			}
		}
	}
	stbds_arrput(dir_names, path2);

	for (i = 0; i < stbds_arrlenu(dir_names); i++) {
#ifdef _WIN32
		// Maybe I'll add code for dealing with security attributes
		// later
		if (!CreateDirectoryA(dir_names[i], NULL))
			errno = purpl_win32_error_to_errno(GetLastError());
#else // _WIN32
		int fd = mkdir(dir_names[i],
			       purpl_translate_file_mode(mode, true));
		close(fd);
#endif // _WIN32
		free(dir_names[i]);
	}

	if (errno)
		return false;

	stbds_arrfree(dir_names);
	return true;
}

PURPL_API char *purpl_path_directory(const char *path, size_t *size,
				     bool relative, bool data_relative)
{
	char *buf;
	char *buf2;
	char *p;

	if (!path)
		return NULL;

	buf = purpl_pathfmt(size, path, 0, relative, data_relative);
	if (!buf) {
		if (size)
			*size = 0;
		return NULL;
	}

	p = strrchr(buf, '/');
	*p = '\0';
	buf2 = purpl_strndup(buf, p - buf + 1);

	if (size)
		*size = p - buf + 1;
	free(buf);
	return buf2;
}

PURPL_API char *purpl_path_file(const char *path, size_t *size, bool relative,
				bool data_relative)
{
	char *buf;
	char *buf2;
	char *p;

	if (!path)
		return NULL;

	buf = purpl_pathfmt(size, path, 0, relative, data_relative);
	if (!buf) {
		if (size)
			*size = 0;
		return NULL;
	}

	if (!strrchr(path, '/')) {
		if (size)
			*size = 0;
		free(buf);
		return purpl_strdup(path);
	}

	p = strrchr(buf, '/') + 1;
	buf2 = purpl_strdup(p);

	if (size)
		*size = strlen(buf2) + 1;
	free(buf);
	return buf2;
}

PURPL_API void purpl_stat(const char *path, struct purpl_file_info *info,
			  bool relative, bool data_relative)
{
	char *path2;
	char *file = NULL;

	if (!path || !info)
		return;

	path2 = purpl_pathfmt(NULL, path, 0, relative, data_relative);

#ifdef _WIN32
#else // _WIN32
	struct stat sb;

	stat(path2, &sb);
	info->size = sb.st_size;
	info->attrs = purpl_translate_file_attrs(sb.st_mode, false);
	info->mode = purpl_translate_file_mode(sb.st_mode, false);
#if defined __APPLE__
	info->ctime = sb.st_ctimespec.tv_sec;
	info->atime = sb.st_atimespec.tv_sec;
	info->mtime = sb.st_mtimespec.tv_sec;
#elif defined __linux__
	info->ctime = sb.st_ctim.tv_sec;
	info->atime = sb.st_atim.tv_sec;
	info->mtime = sb.st_mtim.tv_sec;
#endif // __APPLE__

	file = purpl_path_file(path2, NULL, relative, data_relative);
	if (!file) {
		free(path2);
		return;
	}

	if (file[0] == '.')
		info->attrs |= PURPL_FILE_ATTR_HIDDEN;
#endif // _WIN32

	free(file);
	free(path2);
}

PURPL_API size_t purpl_get_size(const char *path, bool relative,
				bool data_relative)
{
	char *path2;
	struct purpl_file_info info;

	if (!path)
		return 0;

	path2 = purpl_pathfmt(NULL, path, 0, relative, data_relative);
	if (!path2)
		return 0;

	purpl_stat(path2, &info, false, false);
	free(path2);

	return info.size;
}

PURPL_API char *purpl_get_system_data_dir(void)
{
	char *path;
	char *path2;

	path = getenv("PURPL_DATA_DIR");
	if (!path) {
#ifdef _WIN32
		char appdata[MAX_PATH];

		if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appdata) !=
		    ERROR_SUCCESS)
			strncpy(appdata, "C:/", PURPL_SIZEOF_ARRAY(appdata));
		path = appdata;
#else // _WIN32
		if (getenv("HOME"))
			path = purpl_strfmt(NULL, "%s/.local/share", getenv("HOME"));
		else
			path = "/tmp";
#endif // _WIN32
	}

	path = purpl_strdup(path);
	path2 = purpl_pathfmt(NULL, path, 0, false, false);
	free(path);
	return path2;
}
