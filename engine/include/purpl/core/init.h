// Initialization functions
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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "coredefs.h"
#include "types.h"

/// Initialize the engine
///
/// \param log_name Specifies the name of the log file. The string <date> will
/// be replaced with the date in the format <dd>-<mm>-<yyyy>. Can be NULL, in
/// which case the log file will be named purpl-<date>.log
PURPL_API bool purpl_init(const char *log_name);
