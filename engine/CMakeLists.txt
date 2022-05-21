#  Engine CMakeLists
#
#  Copyright 2022 MobSlicer152
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

cmake_minimum_required(VERSION 3.10)

# Enable warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PURPL_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PURPL_WARNINGS}")

# Sources
set(ENGINE_CORE_HEADERS
    include/purpl/core/config.h.in
    include/purpl/core/coredefs.h
    include/purpl/core/init.h
    include/purpl/core/inst.h
    include/purpl/core/log.h
    include/purpl/core/platform.h
    include/purpl/core/types.h)

set(ENGINE_GRAPHICS_HEADERS
    include/purpl/graphics/init.h
    include/purpl/graphics/inst_data.h
    include/purpl/graphics/window.h)

set(ENGINE_VULKAN_HEADERS
    include/purpl/graphics/vulkan/command.h
    include/purpl/graphics/vulkan/debug.h
    include/purpl/graphics/vulkan/device.h
    include/purpl/graphics/vulkan/draw.h
    include/purpl/graphics/vulkan/init.h
    include/purpl/graphics/vulkan/inst_data.h
    include/purpl/graphics/vulkan/inst.h
    include/purpl/graphics/vulkan/pipeline.h
    include/purpl/graphics/vulkan/render.h
    include/purpl/graphics/vulkan/swapchain.h
    include/purpl/graphics/vulkan/sync.h)

set(ENGINE_UTIL_HEADERS
    include/purpl/util/discord.h
    include/purpl/util/error.h
    include/purpl/util/fs.h
    include/purpl/util/misc.h
    include/purpl/util/steam.h
    include/purpl/util/string.h
    include/purpl/util/util.h)

set(ENGINE_CORE_SOURCES
    src/core/globals.c
    src/core/init.c
    src/core/log.c)

set(ENGINE_DEBUG_SOURCES)

set(ENGINE_GRAPHICS_SOURCES
    src/graphics/init.c
    src/graphics/window.c)

set(ENGINE_SHADERS
    src/graphics/shaders/triangle.vert
    src/graphics/shaders/triangle.frag)

set(ENGINE_VULKAN_SOURCES
    src/graphics/vulkan/command.c
    src/graphics/vulkan/debug.c
    src/graphics/vulkan/device.c
    src/graphics/vulkan/draw.c
    src/graphics/vulkan/init.c
    src/graphics/vulkan/inst.c
    src/graphics/vulkan/pipeline.c
    src/graphics/vulkan/render.c
    src/graphics/vulkan/swapchain.c
    src/graphics/vulkan/sync.c)

set(ENGINE_METAL_SOURCES
    src/graphics/metal/init.swift)

set(ENGINE_UTIL_SOURCES
    src/util/error.c
    src/util/fs.c
    src/util/misc.c
    src/util/stb.c
    src/util/string.c)

set(ENGINE_MINGW_x86_64_SOURCES
    src/util/x86/random_64_mingw.S)

set(ENGINE_WINDOWS_x86_64_SOURCES
    src/util/x86/random_64.asm)

set(ENGINE_MACOS_x86_64_SOURCES
    src/util/x86/random_64_mac.S)

set(ENGINE_UNIX_x86_64_SOURCES
    src/util/x86/random_64.S)

set(ENGINE_MINGW_x86_64_SOURCES
    src/util/x86/random_64_mingw.S)

set(ENGINE_UNIX_arm64_SOURCES
    src/util/arm/random_64.S)

set(ENGINE_WINDOWS_SOURCES src/engine.rc)

if (APPLE)
	list(APPEND ENGINE_GRAPHICS_HEADERS ${ENGINE_METAL_HEADERS})
	list(APPEND ENGINE_GRAPHICS_SOURCES ${ENGINE_METAL_SOURCES})
else()
	list(APPEND ENGINE_GRAPHICS_HEADERS ${ENGINE_VULKAN_HEADERS})
	list(APPEND ENGINE_GRAPHICS_SOURCES ${ENGINE_VULKAN_SOURCES})
endif()

if (MINGW)
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_MINGW_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_MINGW_SOURCES} ${ENGINE_MINGW_${PURPL_PROCESSOR}_SOURCES})
elseif (WIN32)
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_WINDOWS_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_WINDOWS_SOURCES} ${ENGINE_WINDOWS_${PURPL_PROCESSOR}_SOURCES})
elseif (APPLE)
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_MACOS_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_MACOS_SOURCES} ${ENGINE_MACOS_${PURPL_PROCESSOR}_SOURCES})
else()
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_UNIX_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_UNIX_SOURCES} ${ENGINE_UNIX_${PURPL_PROCESSOR}_SOURCES})
endif()

if (${PURPL_ENABLE_DISCORD})
	set(ENGINE_UTIL_HEADERS ${ENGINE_UTIL_HEADERS} include/purpl/util/discord.h)
	set(ENGINE_UTIL_SOURCES ${ENGINE_UTIL_SOURCES} src/util/discord.c)
endif()

if (${PURPL_ENABLE_STEAM})
	set(ENGINE_UTIL_HEADERS ${ENGINE_UTIL_HEADERS} include/purpl/util/steam.h)
	set(ENGINE_UTIL_SOURCES ${ENGINE_UTIL_SOURCES} src/util/steam.c)
endif()

set(ENGINE_ALL_HEADERS
    include/purpl/purpl.h
    ${ENGINE_CORE_HEADERS}
    ${ENGINE_GRAPHICS_HEADERS}
    ${ENGINE_UTIL_HEADERS}
    ${ENGINE_PLATFORM_HEADERS})

set(ENGINE_ALL_SOURCES
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_GRAPHICS_SOURCES}
    ${ENGINE_UTIL_SOURCES}
    ${ENGINE_PLATFORM_SOURCES})

# IDE source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${ENGINE_ALL_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${ENGINE_ALL_SOURCES})

# Configuration header
configure_file(include/purpl/core/config.h.in config.h @ONLY)

if (${PURPL_STATIC_BUILD})
	set(engine_lib_type STATIC)
else()
	# Header with dumped exports for engine_preinit
	set(EXPORTS_HEADER ${CMAKE_CURRENT_BINARY_DIR}/exports.h)
	set(engine_lib_type SHARED)
endif()

# Add the two libraries
add_library(engine_preinit OBJECT src/core/main.c src/core/preinit.cc ${CMAKE_SOURCE_DIR}/tools/dumppe.py ${CMAKE_SOURCE_DIR}/tools/dumpelf.py ${CMAKE_SOURCE_DIR}/tools/dumpmacho.py ${EXPORTS_HEADER})
add_library(engine ${engine_lib_type} ${ENGINE_ALL_HEADERS} ${ENGINE_ALL_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Libraries and preprocessor definitions
if (${PURPL_ENABLE_MEMING})
	target_compile_definitions(engine PRIVATE PURPL_ENABLE_MEMING=1)
endif()
if (${PURPL_PROCESSOR} MATCHES "x86_64")
	target_compile_definitions(engine PRIVATE PURPL_X86=1 PURPL_X86_64=1)
elseif(${PURPL_PROCESSOR} MATCHES "x86")
	target_compile_definitions(engine PRIVATE PURPL_X86=1)
elseif(${PURPL_PROCESSOR} MATCHES "arm64")
	target_compile_definitions(engine PRIVATE PURPL_ARM=1)
endif()
target_compile_definitions(engine_preinit PRIVATE _CLANG_DISABLE_CRT_DEPRECATION_WARNINGS=1 _CRT_SECURE_NO_WARNINGS=1 PURPL_BUILD=1)
target_compile_definitions(engine PRIVATE _CLANG_DISABLE_CRT_DEPRECATION_WARNINGS=1 _CRT_SECURE_NO_WARNINGS=1 PURPL_BUILD=1)
if (${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
	target_compile_definitions(engine_preinit PRIVATE PURPL_WINRT=1)
	target_compile_definitions(engine PRIVATE PURPL_WINRT=1)
endif()
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	target_compile_definitions(engine_preinit PRIVATE PURPL_DEBUG=1)
	target_compile_definitions(engine PRIVATE PURPL_DEBUG=1)
endif()
target_link_libraries(engine PRIVATE ${SDL2_LIBRARIES})
if (${PURPL_ENABLE_DISCORD})
	target_compile_definitions(engine PRIVATE PURPL_ENABLE_DISCORD=1)
	target_include_directories(engine PRIVATE ${DISCORD_SDK_INCLUDE_DIRS})
	target_link_libraries(engine PRIVATE discord_game_sdk)
endif()
if (${PURPL_ENABLE_STEAM})
	target_compile_definitions(engine PRIVATE PURPL_ENABLE_STEAM=1)
	target_include_directories(engine PRIVATE ${STEAMWORKS_SDK_INCLUDE_DIRS})
	target_link_libraries(engine PRIVATE steamworks_sdk)
endif()

# Disable exporting every symbol
if(NOT WIN32)
	set_target_properties(engine PROPERTIES C_VISIBILITY_PRESET hidden)
	set_target_properties(engine PROPERTIES CXX_VISIBILITY_PRESET hidden)
endif()


# Dump exports for engine_preinit
if (NOT ${PURPL_STATIC_BUILD})
	if (WIN32)
		add_custom_command(OUTPUT ${EXPORTS_HEADER}
				   COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/dumppe.py $<TARGET_FILE:engine> ${EXPORTS_HEADER} purpl
				   DEPENDS engine ${CMAKE_SOURCE_DIR}/tools/dumppe.py)
	else()
		if (APPLE)
			set(DUMPEXPORTS_NAME dumpmacho.py)
		else()
			set(DUMPEXPORTS_NAME dumpelf.py)
		endif()
		add_custom_command(OUTPUT ${EXPORTS_HEADER}
				   COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/${DUMPEXPORTS_NAME} $<TARGET_FILE:engine> ${EXPORTS_HEADER} purpl
				   DEPENDS engine ${CMAKE_SOURCE_DIR}/tools/${DUMPEXPORTS_NAME})
		target_link_libraries(engine_preinit PUBLIC dl)
	endif()
endif()

# Include directories
target_include_directories(engine_preinit
			   PUBLIC
				${CMAKE_CURRENT_SOURCE_DIR}/include
				${CMAKE_CURRENT_BINARY_DIR}
			   PRIVATE
				${Vulkan_INCLUDE_DIRS}
				${SDL2_INCLUDE_DIRS}
				${CMAKE_SOURCE_DIR}/deps/glad2/include
				${CMAKE_SOURCE_DIR}/deps/sdl2/include
				${CMAKE_SOURCE_DIR}/deps/stb)
target_include_directories(engine
			   PUBLIC
				${CMAKE_CURRENT_SOURCE_DIR}/include
				${CMAKE_CURRENT_BINARY_DIR}
			   PRIVATE
				${Vulkan_INCLUDE_DIRS}
				${SDL2_INCLUDE_DIRS}
				${CMAKE_SOURCE_DIR}/deps/glad2/include
				${CMAKE_SOURCE_DIR}/deps/sdl2/include
				${CMAKE_SOURCE_DIR}/deps/stb)

# Platform specific stuff
if (APPLE)
	set_target_properties(engine
			      PROPERTIES
				XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/purpl/core/swift.h)
else()
	target_link_libraries(engine
			      PRIVATE
				glad_opengl
				glad_vulkan
				${Vulkan_LIBRARIES})
endif()

# No library name prefix
set_target_properties(engine PROPERTIES PREFIX "")
set_target_properties(engine_preinit PROPERTIES PREFIX "")

# Shaders
foreach(shader ${ENGINE_SHADERS})
	string(REPLACE "." "_" shader_name ${shader})
	string(FIND "${shader}" "/" shader_basename_start REVERSE)
	math(EXPR shader_basename_start "${shader_basename_start} + 1" OUTPUT_FORMAT DECIMAL)
	string(SUBSTRING "${shader_name}" ${shader_basename_start} -1 shader_basename)
	add_shader(${shader_basename} ${shader} assets/shaders)
	add_dependencies(engine ${shader_basename})
endforeach()

# This is a list of targets that the engine needs
if (NOT ${PURPL_STATIC_BUILD})
	if (NOT ${SDL2_FOUND})
		list(APPEND ENGINE_RUNTIME_TARGETS_ SDL2)
	endif()
	if (${PURPL_ENABLE_DISCORD})
		list(APPEND ENGINE_RUNTIME_TARGETS_ discord_game_sdk)
	endif()
	if (${PURPL_ENABLE_STEAM})
		list(APPEND ENGINE_RUNTIME_TARGETS_ steamworks_sdk)
	endif()
endif()
set(ENGINE_RUNTIME_TARGETS ${ENGINE_RUNTIME_TARGETS_} PARENT_SCOPE)

if (NOT ${PURPL_STATIC_BUILD})
	# Generate the list of libraries to load
	set(engine_libs ${CMAKE_CURRENT_BINARY_DIR}/engine_libs.txt)
	file(REMOVE ${engine_libs}.in)
	set(engine_libs_content "// List of libraries to be loaded for the engine\n")
	if (${SDL_FOUND})
		foreach (library ${SDL2_LIBRARIES})
			string(APPEND engine_libs_content "${library}\n")
		endforeach()
	endif()
	foreach (target ${ENGINE_RUNTIME_TARGETS_} engine)
		get_target_property(target_imported ${target} IMPORTED)
		if (${target_imported})
			get_target_property(tmp ${target} LOCATION)
			get_filename_component(tmp ${tmp} NAME)
		else()
			file(CONFIGURE OUTPUT ${engine_libs}.tmp CONTENT [=[$<TARGET_FILE_NAME:@target@>]=])
			file(READ ${engine_libs}.tmp tmp)
		endif()
		string(APPEND engine_libs_content "${tmp}\n")
	endforeach()
	file(WRITE ${engine_libs}.in ${engine_libs_content})
	file(GENERATE OUTPUT ${engine_libs} INPUT ${engine_libs}.in)
	install(FILES ${engine_libs} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

# Install targets and symbols
install(TARGETS engine_preinit
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if (${PURPL_STATIC_BUILD})
	set(engine_install_dir lib)
else()
	set(engine_install_dir bin)
endif()
install(TARGETS engine
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${engine_install_dir})

if (MSVC AND ${PURPL_IS_DEBUG})
	install(PROGRAMS $<TARGET_PDB_FILE:engine>
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

