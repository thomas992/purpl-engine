// Window creation
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

#include "purpl/graphics/window.h"

namespace purpl::graphics
{

window::window()
{
	window("Purpl Engine", -1, -1, -1, -1, graphics_api::VULKAN, true, false);
}

window::window(const std::string &title, s32 width, s32 height, s32 x, s32 y, graphics_api api, bool resizeable, bool fullscreen)
{
	GLFWvidmode *mode; // Video mode of the primary monitor

	// Make sure the engine is initialized
	if (!purpl::core::inst) {
		purpl::core::inst->gfx_logger->error("An attempt was made to create a window while the engine seemingly isn't initialized");
		return;
	}

	// Store window stuff
	this->title = title;
	this->width = width;
	this->height = height;
	this->x = x;
	this->y = y;
	this->api = api;
	this->resizeable = resizeable;
	this->fullscreen = fullscreen;

	// See if the default window size should be used
	if (this->width < 0)
		this->width = 1024;
	if (this->height < 0)
		this->height = 768;

	   // Create the window
	this->wnd = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
	if (!this->wnd) {
		purpl::core::inst->gfx_logger->error("Failed to create window");
		return;
	}

	// Set the window's user pointer and callbacks
	glfwSetWindowUserPointer(this->wnd, this);
	glfwSetKeyCallback(this->wnd, glfw_key_callback);

	// Show the window
	glfwShowWindow(this->wnd);

	   // Set things
	this->fullscreen_width = this->width;
	this->fullscreen_height = this->height;
	this->fullscreen_resizeable = this->resizeable;
	if (this->x >= 0 || this->y >= 0)
		this->set_position(this->x, this->y);
	else
		glfwGetWindowPos(this->wnd, &this->x, &this->y);
	this->fullscreen_x = this->x;
	this->fullscreen_y = this->y;
	this->set_resizeable(this->resizeable);
	this->set_fullscreen(this->fullscreen);

	   // Add this window to the global list
	purpl::core::inst->windows.insert(this);

	purpl::core::inst->gfx_logger->info("Created {}x{} window {} titled {} at {} {} successfully", this->width, this->height, (void *)this, this->title, this->x, this->y);
}

window::~window()
{
	glfwDestroyWindow(this->wnd);
	purpl::core::inst->windows.erase(this);
}

bool window::update()
{
	// Swap the window's buffers
	glfwSwapBuffers(this->wnd);

	// Store the window's details
	glfwGetWindowPos(this->wnd, &this->x, &this->y);

	return !glfwWindowShouldClose(this->wnd);
}

void window::set_title(const std::string &title)
{
	this->title = title;
	glfwSetWindowTitle(this->wnd, this->title.c_str());
	purpl::core::inst->gfx_logger->info("Set title of window {} to {}", (void *)this, this->title);
}

void window::set_size(u32 width, u32 height)
{
	this->width = width;
	this->height = height;
	glfwSetWindowSize(this->wnd, this->width, this->height);
	purpl::core::inst->gfx_logger->info("Resized window {} (titled {}) to {}x{}", (void *)this, this->title, this->width, this->height);
}

void window::set_position(u32 x, u32 y)
{
	this->x = x;
	this->y = y;
	glfwSetWindowPos(this->wnd, this->x, this->y);
	purpl::core::inst->gfx_logger->info("Moved window {} (titled {}) to {} {}", (void *)this, this->title, this->x, this->y);
}

void window::set_resizeable(bool resizeable)
{
	if (!this->fullscreen) {
		this->resizeable = resizeable;
		glfwSetWindowAttrib(this->wnd, GLFW_RESIZABLE, this->resizeable);
		purpl::core::inst->gfx_logger->info("Toggled resizing for window {} (titled {})", (void *)this, this->title);
	}
}

void window::set_fullscreen(bool fullscreen)
{
	GLFWvidmode *mode;
	s32 x;
	s32 y;

	   // Set the window's stuff
	this->fullscreen = fullscreen;
	if (this->fullscreen) {
		// Get the video mode for the current monitor
		mode = (GLFWvidmode *)glfwGetVideoMode(glfw_get_monitor_by_position(this->x, this->y));
		if (!mode) {
			purpl::core::inst->gfx_logger->warn("Failed to get video mode for fullscreen");
			return;
		}

		// Save current position and size and set the window monitor
		this->fullscreen_x = this->x;
		this->fullscreen_y = this->y;
		this->fullscreen_width = this->width;
		this->fullscreen_height = this->height;
		glfwSetWindowMonitor(this->wnd, glfw_get_monitor_by_position(this->x, this->y), 0, 0, mode->width, mode->height, mode->refreshRate);
	} else {
		this->set_resizeable(this->fullscreen_resizeable);
		glfwSetWindowMonitor(this->wnd, nullptr, this->fullscreen_x, this->fullscreen_y, this->fullscreen_width, this->fullscreen_height, GLFW_DONT_CARE);
	}

	// Re-show the window
	glfwShowWindow(this->wnd);

	purpl::core::inst->gfx_logger->info("Toggled fullscreen for window {}", (void *)this);
}

void window::set_api(graphics_api api)
{
	this->api = api;
}

GLFWwindow *window::get_handle()
{
	return this->wnd;
}

const std::string &window::get_title()
{
	return this->title;
}

u32 window::get_width()
{
	return this->width;
}

u32 window::get_height()
{
	return this->height;
}

u32 window::get_x()
{
	return this->x;
}

u32 window::get_y()
{
	return this->y;
}

bool window::is_resizeable()
{
	return this->resizeable;
}

bool window::is_fullscreen()
{
	return this->fullscreen;
}

}
