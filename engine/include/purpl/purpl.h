// Header with all the other headers
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

#ifdef PURPL_BUILD
#error "Don't include purpl/purpl.h in engine files, only programs using the engine"
#endif

#include "core/coredefs.h"
#include "core/features.h"
#include "core/init.h"
#include "core/inst.h"
#include "core/types.h"

#include "util/util.h"
