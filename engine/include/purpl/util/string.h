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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

/// Replace all instances of old with new in src and store the result in dst
///
/// \param dst The destination buffer
/// \param src The source string
/// \param old The string to replace
/// \param new The string to replace old with
///
/// \return Returns dst
extern PURPL_API char *purpl_strrplc(char *dst, const char *src, const char *old, const char *new);

/// Like purpl_strrplc, but only process up to n characters
///
/// \param dst The destination buffer
/// \param src The source string
/// \param n The maximum number of characters to put in dst
/// \param old The string to replace
/// \param new The string to replace old with
///
/// \return Returns dst
extern PURPL_API char *purpl_strnrplc(char *dst, const char *src, size_t n, const char *old, const char *new);
