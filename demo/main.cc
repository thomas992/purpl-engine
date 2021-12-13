// Demo of the engine
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

#include <iostream>

#include "purpl/purpl.h"

/// Function to be called each frame
static bool frame(purpl::chrono::milliseconds delta, void *wnd);

int main(int argc, char *argv[])
{
	// Initialize the engine
	if (!purpl::core::init()) {
		std::cerr << "Error: failed to initialize the engine\n";
		exit(1);
	}

	// Create a window. It gets added to an internal list, with a delete run on it when it should close.
	purpl::graphics::window *wnd = new purpl::graphics::window("Purpl Engine Demo", -1, -1, -1, -1, purpl::graphics::graphics_api::VULKAN, true, false);

	// Run the engine
	purpl::core::run(frame, (void *)wnd);

	// Shut down the engine
	purpl::core::shutdown();

	return 0;
}

static bool frame(purpl::chrono::milliseconds delta, void *wnd)
{
	return true;
}
