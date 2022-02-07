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

#pragma once

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

#include "error.h"
#include "string.h"

/// File modes (that matter in the slightest to the engine and exist on POSIX
/// and NT)
enum purpl_file_mode {
	PURPL_FS_MODE_NONE = 0b0, // No permissions (default)
	PURPL_FS_MODE_READ = 0b1, // Read
	PURPL_FS_MODE_WRITE = 0b10, // Write
	PURPL_FS_MODE_EXECUTE = 0b100, // Execute
	PURPL_FS_MODE_RDWR = 0b11, // Read-write
	PURPL_FS_MODE_RDXC = 0b101, // Read and execute
	PURPL_FS_MODE_ALL = 0b111, // Read, write, and execute
	PURPL_FS_MODE_EVERYONE = 0b1000, // Allows group and others to write
};

/// File attributes
enum purpl_file_attrs {
	PURPL_FILE_ATTR_NORMAL = 0b0, // A normal file
	PURPL_FILE_ATTR_DEVICE = 0b1, // A device file
	PURPL_FILE_ATTR_DIRECTORY = 0b10, // A directory
	PURPL_FILE_ATTR_SYMLINK = 0b100, // A symbolic link
	PURPL_FILE_ATTR_PIPE = 0b1000, // A pipe
	PURPL_FILE_ATTR_HIDDEN = 0b10000, // A hidden file (on POSIX systems,
					// determined by the file name)
	PURPL_FILE_ATTR_READONLY = 0b100000, // Read-only (on POSIX systems,
					   // determined by file mode)
};

/// Flags for filesystem functions
enum purpl_fs_flags {
	PURPL_FS_MKDIR_RECURSE = 0b1, // Create directories recursively
};

/// File information
struct purpl_file_info {
	size_t size; // File size
	u16 attrs; // File attributes
	u16 mode; // File mode
	time_t ctime; // Creation time
	time_t atime; // Access time
	time_t mtime; // Modification time
};

/// Translates between a platform-specific file mode and one used by the engine
///
/// \param mode The mode to translate
/// \param to_native Whether to convert to or from a native mode
///
/// \return Returns the translated mode
extern PURPL_API u32 purpl_translate_file_mode(u32 mode, bool to_native);

/// Translates between platform-specific file attributes and ones used by the engine
///
/// \param attrs The attributes to translate
/// \param to_native Whether to convert to or from native attributes
///
/// \return Returns the translated attributes
extern PURPL_API u32 purpl_translate_file_mode(u32 attrs, bool to_native);

/// Formats a path (you don't need to call this)
///
/// \param size The size of the formatted path
/// \param path The path to format
/// \param flags The flags to apply
extern PURPL_API char *purpl_pathfmt(size_t *size, const char *path,
				     enum purpl_fs_flags flags);

/// Checks if a file exists
///
/// \param path The path to check
///
/// \return Returns true if the file exists, false otherwise
extern PURPL_API bool purpl_path_exists(const char *path);

/// Create a directory
///
/// \param path The path of the directory
/// \param flags The flags for the directory's creation
/// \param mode The mode for the directory
///
/// \return Returns true on success, and sets errno on Windows
extern PURPL_API bool purpl_mkdir(const char *path, enum purpl_fs_flags flags,
				  enum purpl_file_mode mode);

/// Get the directory component of a path
///
/// \param path The path
/// \param size The size of the buffer, optional
///
/// \return Returns the directory name in a new buffer, or NULL on failure
extern PURPL_API char *purpl_path_directory(const char *path, size_t *size);

/// Get the file component of a path
///
/// \param path The path
/// \param size The size of the buffer, optional
///
/// \return Returns the file name in a new buffer, or NULL on failure
extern PURPL_API char *purpl_path_file(const char *path, size_t *size);

/// stat abstraction
///
/// \param path The path to stat
/// \param info A purpl_file_info struct to store the information in. Will be
/// 		NULL on failure
extern PURPL_API void purpl_stat(const char *path,
				 struct purpl_file_info *info);

/// Get the size of a file
///
/// \param path The path to check
///
/// \return Returns the size of the file
extern PURPL_API size_t purpl_get_size(const char *path);
