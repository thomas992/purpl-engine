// Preprocessor maze to detect features like 128-bit integers
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

// Clang and GCC support these and have for like a decade, while MSVC is still
// too lazy to implement them
#if defined __clang__ || defined __GNUC__
// Defined if 128-bit integers are available
#define PURPL_INT128_AVAILABLE 1
#endif // __clang__ || __GNUC__
