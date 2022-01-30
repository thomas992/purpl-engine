// Functions to make use of NTDLL
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

#ifndef PURPL_BUILD
#error This file is only meant to be included by the engine
#endif // PURPL_BUILD

#define PHNT_VERSION PHNT_THRESHOLD
#include <phnt_windows.h>
#include <phnt.h>

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

#include "purpl/util/string.h"

/// The base address of NTDLL
extern void *ntdll_base;

/// Functions loaded
extern u32 (*DbgPrint_l)(const char *fmt, ...); /// DbgPrint

/// Finds NTDLL's base address in the PEB
extern void purpl_load_ntdll(void);
