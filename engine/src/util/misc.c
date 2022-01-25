// Miscellaneous utility functions
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
#else
#define _GNU_SOURCE
#include <unistd.h>
#endif

#include "purpl/util/misc.h"

inline u32 purpl_get_pid(void)
{
#ifdef _WIN32
	return (u32)(size_t)(NtCurrentTeb()->ClientId.UniqueProcess);
#else
	return getpid();
#endif // _WIN32
}

inline u32 purpl_get_tid(void)
{
#ifdef _WIN32
	return (u32)(size_t)(NtCurrentTeb()->ClientId.UniqueThread);
#else
	return gettid();
#endif // _WIN32
}
