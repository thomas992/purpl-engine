// Extra string manipulation functions
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

/// Count all instances of sub in str
///
/// \param str The string to search
/// \param sub The substring to count instances of
///
/// \return Returns the number of instances
extern PURPL_API size_t purpl_strcount(const char *str, const char *sub);

/// Replace all instances of old in str with new and place the result in a new
/// buffer
///
/// \param str The string to process
/// \param old The substring to replace
/// \param new The substring to replace old with
///
/// \return Returns the new string, which can be freed with free
extern PURPL_API char *purpl_strrplc(const char *str, const char *old, const char *new, size_t *size);

/// Replace the next instance of old with new
///
/// \param str The string to process
/// \param old The substring to replace
/// \param new The substring to replace the first instance of old with
///
/// \return Returns the new string, which can be freed with free
extern PURPL_API char *purpl_strrplcn(const char *str, const char *old, const char *new, size_t *size);

/// Formats a string into a buffer and takes care of determining the needed
/// size of the buffer
///
/// \param size A pointer in which the size of the buffer is returned if it
/// isn't NULL
/// \param fmt The printf format string to use
///
/// \return Returns a string which can be freed using free
extern PURPL_API char *purpl_strfmt(size_t *size, const char *fmt, ...);

/// Formats a string into a buffer and takes care of determining the needed
/// size of the buffer
///
/// \param size A pointer in which the size of the buffer is returned if it
/// isn't NULL
/// \param fmt The printf format string to use
/// \param args The varargs list to use
///
/// \return Returns a string which can be freed using free
extern PURPL_API char *purpl_vstrfmt(size_t *size, const char *fmt, va_list args);

/// Duplicate a string like strdup(3)
///
/// \param str The string to duplicate
///
/// \return Returns a buffer containing a copy of str including the NUL
///	    terminator or NULL
extern PURPL_API char *purpl_strdup(const char *str);

/// Duplicate a string like strndup(3)
///
/// \param str The string to duplicate
/// \param n The maximum number of characters to copy from str
///
/// \return Returns a buffer containing n characters of str and a NUL
///	    terminator, or NULL
extern PURPL_API char *purpl_strndup(const char *str, size_t n);

/// Insert ins into str at index
///
/// \param str The string to insert into
/// \param ins The string to insert
/// \param index The index at which to insert ins
/// \param size The size of the buffer
///
/// \return Returns a buffer containing str with ins inserted at index and its
///	    size increased by strlen(ins) from size, or NULL
extern PURPL_API char *purpl_strins(const char *str, const char *ins, size_t index, size_t size);

/// Remove n characters from string at index
///
/// \param str The string to remove from
/// \param index The index to start at
/// \param n The number of characters to remove
/// \param size The size of the buffer
///
/// \return Returns a buffer containing str with n characters removed from
///	    index and its size decreased by n from size, or NULL
extern PURPL_API char *purpl_strrm(const char *str, size_t index, size_t n, size_t size);

/// Perform a case insensitive string comparison on two wide strings like
/// wcscasecmp(3)
///
/// \param s1 The first string
/// \param s2 The second string
///
/// \return Returns the difference between the last two characters compared
/// rounded to -1, 0, or 1
extern PURPL_API s8 purpl_wcscasecmp(const wchar_t *s1, const wchar_t *s2);

/// Converts a version number into a string in the format "major.minor.patch"
///
/// \param version The version number (from PURPL_MAKE_VERSION) to format
///
/// \return Returns a static buffer with PURPL_STATIC_BUF_MAX bytes of space
extern PURPL_API const char *purpl_format_version(u32 version);

#ifdef PURPL_BUILD
/// Gets the name of the specified graphics API
///
/// \param api The graphics API to get the name of
///
/// \return Returns a string literal, don't write to it or free it
extern const char *purpl_get_graphics_api_name(u32 api);
#endif // PURPL_BUILD
