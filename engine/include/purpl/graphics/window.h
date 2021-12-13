// Window abstraction
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

#include <algorithm>
#include <memory>
#include <string>

#ifdef PURPL_ENABLE_VULKAN
// Tell GLFW to include the Vulkan API header
#define GLFW_INCLUDE_VULKAN

// Include the C++ Vulkan header
#include <vulkan/vulkan.hpp>
#endif

#include <GLFW/glfw3.h>

#include "purpl/core/coredefs.h"

#include "graphics.h"

namespace purpl::graphics
{

/// Class to abstract a GLFW window. MUST be allocated with new
class PURPL_API window
{	
    public:
	/// Default constructor. Width and height default to 1024x768
	window();

	/// Constructor. Negative width, hight, X, and Y values mean don't move the
	/// window from the default position and use 1024x768 as the resolution
	window(const std::string &title, s32 width, s32 height, s32 x, s32 y,
	       graphics_api api, bool resizeable, bool fullscreen);

	/// Destructor. Closes the window
	~window();

	/// Update the window. Returns true if the window should remain open
	bool update();

	/// Set the window's title
	void set_title(const std::string &title);

	/// Set the window's size
	void set_size(u32 width, u32 height);

	/// Set the window's position
	void set_position(u32 x, u32 y);

	/// Change whether the window is resizeable or not
	void set_resizeable(bool resizeable);

	/// Set whether the window is fullscreened
	void set_fullscreen(bool fullscreen);

	/// Reinitialize a different graphics API
	void set_api(graphics_api api);

	/// Get the GLFW window handle
	GLFWwindow *get_handle();

	/// Get the current title
	const std::string &get_title();

	/// Get the current width
	u32 get_width();

	/// Get the current height
	u32 get_height();

	/// Get the current X position
	u32 get_x();

	/// Get the current Y position
	u32 get_y();

	/// Get whether the window can be resized
	bool is_resizeable();

	/// Get whether the window is fullscreened
	bool is_fullscreen();

    private:
	GLFWwindow *wnd; /// GLFW window
	std::string title; /// Window title
	s32 width; /// Width
	s32 fullscreen_width; /// Pre-fullscreen width
	s32 height; /// Height
	s32 fullscreen_height; /// Pre-fullscreen height
	s32 x; /// Current X position
	s32 fullscreen_x; /// Pre-fullscreen X position
	s32 y; /// Current Y position
	s32 fullscreen_y; /// Pre-fullscreen Y position
	graphics_api api; /// Currently initialized graphics API
	bool resizeable; /// Whether the window is resizeable
	bool fullscreen_resizeable; /// Whether window could be resized pre-fullscreen
	bool fullscreen; /// Whether the window is fullscreened
};

}
