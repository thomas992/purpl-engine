// Miscellaneous utility functions for GLFW
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

#include "purpl/graphics/graphics.h"
#include "purpl/graphics/window.h"

namespace purpl::graphics
{

void glfw_report_error(s32 code, const char *desc)
{
	purpl::core::inst->gfx_logger->error("GLFW error: {} (code {})", desc, code);
}

void glfw_key_callback(GLFWwindow *wnd, s32 key, s32 scancode, s32 action, s32 mode)
{
	window *engine_wnd;

	// Check if the window came from the engine
	engine_wnd = (window *)glfwGetWindowUserPointer(wnd);
	if (!engine_wnd) {
		purpl::core::inst->gfx_logger->error("Engine key callback registered for non-engine window {}", (void *)wnd);
		return;
	}

	switch (key) {
	// Toggle fullscreen
	case GLFW_KEY_F11:
		if (action == GLFW_PRESS)
			engine_wnd->set_fullscreen(!engine_wnd->is_fullscreen());
		break;
	}
}

GLFWmonitor *glfw_get_monitor_by_position(s32 x, s32 y)
{
	GLFWmonitor **monitors;
	s32 monitor_count;
	s32 i;

	// Get the monitors
	monitors = glfwGetMonitors(&monitor_count);
	if (!monitors) {
		purpl::core::inst->gfx_logger->error("Failed to get list of monitors");
		return glfwGetPrimaryMonitor();
	}

	// Go through the monitors
	for (i = 0; i < monitor_count; i++) {
		s32 monitor_x;
		s32 monitor_y;
		GLFWvidmode *mode;

		// Get the position and size
		glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);
		mode = (GLFWvidmode *)glfwGetVideoMode(monitors[i]);

		// Check the position
		if ((x >= monitor_x && y >= monitor_y) && (x <= monitor_x + mode->width && y <= monitor_y + mode->height))
			return monitors[i];
	}

	// Return the primary monitor by default
	return glfwGetPrimaryMonitor();
}

}
