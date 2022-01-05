// Extra string manipulation functions
//
// Copyright 2021 MobSlicer152
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
extern PURPL_API char *purpl_strrplc(const char *str, const char *old,
				     const char *new, size_t *size);

/// Replace the next instance of old with new
///
/// \param str The string to process
/// \param old The substring to replace
/// \param new The substring to replace the first instance of old with
///
/// \return Returns the new string, which can be freed with free
extern PURPL_API char *purpl_strrplcn(const char *str, const char *old,
				      const char *new, size_t *size);

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
extern PURPL_API char *purpl_vstrfmt(size_t *size, const char *fmt,
				     va_list args);

/// The size of the (read only) buffer returned by purpl_strerror
#define PURPL_STRERROR_BUF_MAX 128

/// Gets an error string from the current errno value
///
/// \return Returns an error string in a static buffer with
///	    PURPL_STRERROR_BUF_MAX bytes of space
extern PURPL_API const char *purpl_strerror(void);
