// Functions and definitions related to errors
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

#include "purpl/util/misc.h"
#include "purpl/util/string.h"

#ifdef _WIN32
/// Translates a Win32 error code to an errno value
///
/// \param error The Win32 error code
///
/// \return The errno value
PURPL_API u32 purpl_win32_error_to_errno(u32 error);

/// Translates an errno value to a Win32 error code
///
/// \param error The errno value
///
/// \return The Win32 error code
PURPL_API u32 purpl_errno_to_win32_error(u32 error);
#endif // _WIN32

/// Gets an error string from the current errno value
///
/// \return Returns an error string in a static buffer with
///	    PURPL_STATIC_BUF_MAX bytes of space
extern PURPL_API const char *purpl_strerror(void);
