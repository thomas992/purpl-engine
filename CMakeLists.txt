#  Build configuration logic
#
#  Copyright 2021 MobSlicer152
#  This file is part of Purpl Engine
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

cmake_minimum_required(VERSION 3.18)

project(purpl CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Debug build
set(CMAKE_BUILD_TYPE Debug)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_CURRENT_LIST_DIR}/tools/vcpkg/scripts/buildsystems/vcpkg.cmake)

# Death to macOS
if (APPLE)
	message(FATAL_ERROR "macOS is immensely painful to support, please test on a better operating system")
endif()


# Dependencies
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")
find_package(tomlplusplus CONFIG REQUIRED)

# Options
option(PURPL_BUILD_DEMO "Whether to build the feature demo" ON)
option(PURPL_ENABLE_OPENGL_SUPPORT "Whether to enable OpenGL support" ON)
option(PURPL_ENABLE_VULKAN_SUPPORT "Whether to enable Vulkan support" ON)

# Include directories for all targets
include_directories(${FMT_INCLUDE_DIRS};${GLFW_INCLUDE_DIRS};${GLM_INCLUDE_DIRECTORIES};${SPDLOG_INCLUDE_DIRS};${STB_INCLUDE_DIRS};${TOMLPLUSPLUS_INCLUDE_DIRS};engine/include)
add_compile_definitions(FMT_HEADER_ONLY=1)

# OpenGL dependencies
if (${PURPL_ENABLE_OPENGL_SUPPORT})
	message(STATUS "Enabling OpenGL support")
	find_package(GLEW REQUIRED)
	find_package(OpenGL REQUIRED)
	add_compile_definitions(PURPL_ENABLE_OPENGL=1)
endif()

# Vulkan dependencies
if (${PURPL_ENABLE_VULKAN_SUPPORT})
	message(STATUS "Enabling Vulkan support")
	find_package(Vulkan REQUIRED)
	add_compile_definitions(PURPL_ENABLE_VULKAN=1)
endif()

# Add the engine and possibly the demo
add_subdirectory(engine)
if (${PURPL_BUILD_DEMO})
	message(STATUS "Enabling the demo")
	add_subdirectory(demo)
endif()
