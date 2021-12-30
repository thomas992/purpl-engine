// Definition of the engine instance structure
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

#include <time.h>

#include "coredefs.h"
#include "log.h"
#include "types.h"

#ifdef PURPL_BUILD
// Structure to hold information about the current instance of the engine
struct purpl_instance {
	struct purpl_logger *logger; // The logger for this instance
	time_t start_time; // When the engine started
};
#endif
