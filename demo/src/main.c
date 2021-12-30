// A demo of the engine's features
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

#include <purpl/purpl.h>

int32_t main(int32_t argc, char *argv[])
{
	struct purpl_logger *logger;

	logger = purpl_log_create("test-<date>.log", PURPL_LOG_LEVEL_DEBUG, PURPL_LOG_LEVEL_MAX, "#def");
	PURPL_LOG_CURRENT(logger, "asdf %s", "fdsa");

	return 0;
}
