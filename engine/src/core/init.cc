// Initialization function implementations
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

#include <GLFW/glfw3.h>

#include "purpl/core/init.h"

#include "purpl/graphics/graphics.h"
#include "purpl/graphics/window.h"

namespace purpl::core
{

PURPL_API bool init(void)
{
	bool ret;

	// Allocate the instance
	inst = std::make_unique<engine_instance>();

	// Initialize GLFW
	ret = glfwInit();
	if (!ret) {
		inst->core_logger->critical("Failed to initialize GLFW! Bailing on initialization. Error code is {}", ret);
		return false;
	}

	// Set the GLFW error callback
	glfwSetErrorCallback(purpl::graphics::glfw_report_error);

	// Say that the engine has initialized
	inst->core_logger->info("Engine started");

	return true;
}

PURPL_API void run(frame_callback user_frame, void *user_ptr)
{
	bool running = true;
	chrono::time_point<chrono::high_resolution_clock> now;
	chrono::time_point<chrono::high_resolution_clock> last;

	// Set the times
	now = chrono::high_resolution_clock::now();
	last = now;

	// Run
	inst->core_logger->info("Entering engine loop");
	while (running) {
		// Process GLFW events
		glfwPollEvents();

		// Call the user's frame function
		user_frame(chrono::duration_cast<chrono::milliseconds>(now - last), user_ptr);

		// Update all the windows
		for (auto wnd : inst->windows) {
			if (!wnd->update())
				delete wnd;
		}

		// Make sure there are still open windows
		if (inst->windows.empty()) {
			inst->gfx_logger->warn("No remaining windows, exiting loop");
			break;
		}

		// Wait
		std::this_thread::sleep_for(chrono::seconds(1 / 60));

		// Store the time
		last = now;
		now = chrono::high_resolution_clock::now();
	}
}

PURPL_API void shutdown(void)
{
	inst->core_logger->warn("Engine shutting down!");

	// Shut down GLFW
	glfwTerminate();

	inst.reset();
}

}
