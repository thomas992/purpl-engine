#  Engine targets
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

set(PURPL_CORE_HEADERS
	include/purpl/core/coredefs.h
	include/purpl/core/features.h
	include/purpl/core/init.h
	include/purpl/core/inst.h
	include/purpl/core/log.h
	include/purpl/core/types.h
)

set(PURPL_GRAPHICS_HEADERS
	include/purpl/graphics/graphics.h
	include/purpl/graphics/window.h
)

set(PURPL_VULKAN_HEADERS
	include/purpl/graphics/vulkan.h

	include/purpl/graphics/vulkan/init.h
)

set(PURPL_CORE_SOURCES
	src/core/init.cc
	src/core/inst.cc
)

set(PURPL_GRAPHICS_SOURCES
	src/graphics/window.cc
	src/graphics/util.cc
)

set(PURPL_VULKAN_SOURCES
	src/graphics/vulkan/init.cc
)

set(PURPL_ALL_HEADERS include/purpl/purpl.h ${PURPL_CORE_HEADERS} ${PURPL_GRAPHICS_HEADERS})
set(PURPL_ALL_SOURCES ${PURPL_CORE_SOURCES} ${PURPL_GRAPHICS_SOURCES})

if (${PURPL_ENABLE_VULKAN_SUPPORT})
	list(APPEND PURPL_ALL_HEADERS ${PURPL_VULKAN_HEADERS})
	list(APPEND PURPL_ALL_SOURCES ${PURPL_VULKAN_SOURCES})
endif()

add_library(purpl SHARED ${PURPL_ALL_HEADERS} ${PURPL_ALL_SOURCES})
set_target_properties(purpl PROPERTIES COMPILE_DEFINITIONS "PURPL_BUILD")
target_link_libraries(purpl PRIVATE fmt::fmt glfw glm::glm spdlog::spdlog tomlplusplus::tomlplusplus)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/include/purpl PREFIX "Header Files" FILES ${PURPL_ALL_HEADERS})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX "Source Files" FILES ${PURPL_ALL_SOURCES})

if (${PURPL_ENABLE_OPENGL_SUPPORT})
        target_link_libraries(purpl PRIVATE GLEW::GLEW OpenGL::GL)
endif()

if (${PURPL_ENABLE_VULKAN_SUPPORT})
	target_link_libraries(purpl PRIVATE Vulkan::Vulkan)
endif()
