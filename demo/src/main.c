// A demo of the engine's features
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

#include <errno.h>

#include <purpl/purpl.h>

// This will be called each frame by the engine
bool frame_func(s32 delta, void *data);

int32_t main(int32_t argc, char *argv[])
{
	// Initialize the engine
	if (!purpl_init("purpl-demo", PURPL_MAKE_VERSION(1, 0, 0))) {
		fprintf(stderr, "Error: failed to initialize the engine: %s",
			purpl_strerror());
		exit(errno);
	}

	// Run the main loop
	purpl_run(frame_func, NULL);

	// Shut down the engine
	purpl_shutdown();

	return 0;
}

bool frame_func(s32 delta, void *data)
{
	return true;
}
