// Definitions used by graphics stuff
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

#pragma once

#include "purpl/core/coredefs.h"
#include "purpl/core/inst.h"

#include <GLFW/glfw3.h>

namespace purpl::graphics
{

/// Graphics APIs
enum graphics_api {
	SOFTWARE, /// Software rendering, I bought a book but I want to do Vulkan first
	OPENGL, /// OpenGL
	VULKAN, /// Vulkan
	END /// Max value
};

/// Logs an error
extern void glfw_report_error(s32 code, const char *desc);

/// Key callback
extern void glfw_key_callback(GLFWwindow *wnd, s32 key, s32 scancode, s32 action, s32 mode);

/// Get a monitor by position
extern GLFWmonitor *glfw_get_monitor_by_position(s32 x, s32 y);

}
