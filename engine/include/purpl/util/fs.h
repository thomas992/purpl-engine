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
};

/// Flags for filesystem functions
enum purpl_fs_flags {
	PURPL_FS_MKDIR_RECURSE = 0b1, // Create directories recursively
	PURPL_FS_EXCLUSIVE_ACCESS = 0b10, // Gain exclusive access to the file
};

/// Translates between a platform specific file mode and one used by the engine
///
/// \param mode The mode to translate
/// \param to_native Whether to convert to or from a native mode
///
/// \return Returns the translated mode
extern PURPL_API u32 purpl_translate_mode(u32 mode, bool to_native);

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
