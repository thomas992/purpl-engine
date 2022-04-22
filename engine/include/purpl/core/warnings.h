// Warnings
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

#include "features.h"

#ifdef PURPL_RESET_WARNINGS
#if defined __clang__ // __clang__
#pragma clang diagnostic warning "-Wall"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wpedantic"
#elif defined _MSC_VER // __clang__
#pragma warning(default:)
#endif // __clang__
#else // PURPL_RESET_WARNINGS
#if defined __clang__ // __clang__
#pragma clang diagnostic error "-Wall"
#pragma clang diagnostic error "-Wextra"
#pragma clang diagnostic error "-Wpedantic"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"
#elif defined _MSC_VER // __clang__
#pragma warning(4 :)
#pragma warning(error:)
#endif // __clang__
#endif // PURPL_RESET_WARNINGS