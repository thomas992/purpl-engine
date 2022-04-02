// A demo of using the engine
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

// A structure to store information for the application
struct demo_info {
	double frames; // Number of times frame_func was called
	double deltas; // Sum of every frame delta
};

// This will be called each frame by the engine
bool frame_func(u32 delta, void *data);

int32_t main(int32_t argc, char *argv[])
{
	struct demo_info info = { 0 };
	
	// Make sure the engine is ready (loads the DLL on Windows)
	purpl_preinit(argc, argv);

	// Initialize the engine
	if (!purpl_init("purpl-demo", PURPL_MAKE_VERSION(1, 0, 0))) {
		fprintf(stderr, "Error: failed to initialize the engine: %s\n",
			purpl_strerror());
		exit(errno);
	}

	// Run the main loop, it takes a void pointer passed to the frame function
	purpl_run(frame_func, &info);

	// Shut down the engine
	purpl_shutdown();

	printf("Rendered a total of %lf frames, with an average delta of %lf\n", info.frames, info.deltas / info.frames);
	
	return 0;
}

bool frame_func(u32 delta, void *data)
{
	struct demo_info *info = data;
	
	// Tabs are to prevent characters from staying behind
	printf("\rFrame delta: %ums   ", delta);
	
	info->deltas += delta;
	info->frames++;
	
	return true;
}
