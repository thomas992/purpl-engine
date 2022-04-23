#  Main CMakeLists
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

cmake_minimum_required(VERSION 3.15)

include(cmake/GetGitRevisionDescription.cmake)
include(cmake/Platform.cmake)
include(cmake/SDKs.cmake)

if (APPLE AND NOT ${CMAKE_GENERATOR} MATCHES "Xcode")
	message(FATAL_ERROR "Only Xcode is supported on Apple platforms due to otherwise incomplete compatibility with mixing C and Swift code in CMake")
endif()

# Use the engine's Ninja
if (${CMAKE_GENERATOR} MATCHES "Ninja")
	if (WIN32)
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja.exe CACHE STRING "" FORCE)
	else()
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja CACHE STRING "" FORCE)
	endif()
endif()

# Don't link to anything
file(WRITE ${CMAKE_BINARY_DIR}/MakeRulesOverwrite.cmake
[=[
if (MSVC AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
	set(DEFAULT_C_LIBRARIES ${CMAKE_C_STANDARD_LIBRARIES_INIT})
	set(DEFAULT_CXX_LIBRARIES ${CMAKE_C_STANDARD_LIBRARIES_INIT})
	set(CMAKE_C_STANDARD_LIBRARIES_INIT "kernel32.lib ntdll.lib shell32.lib user32.lib")
	set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "kernel32.lib ntdll.lib shell32.lib user32.lib")
endif()
]=]
)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_BINARY_DIR}/MakeRulesOverwrite.cmake")

# Declare the project
project(purpl-engine VERSION 1.0.0 LANGUAGES C CXX)

if (NOT DEFINED CMAKE_SYSTEM_PROCESSOR OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "")
	set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif()

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "amd64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	set(PURPL_PROCESSOR "x86_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "86")
	set(PURPL_PROCESSOR "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "a.*64")
	set(PURPL_PROCESSOR "arm64")
endif()

if (WIN32)
	enable_language(ASM_MASM)
else()
	enable_language(ASM)
endif()
if (APPLE)
	enable_language(Swift)
endif()

cmake_policy(SET CMP0079 NEW)

# Settings
if (NOT DEFINED CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
	message(WARNING "CMAKE_BUILD_TYPE was not set, setting it to Debug")
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
option(PURPL_ENABLE_DEMO "Build the demo" ON)
option(PURPL_ENABLE_MEMING "Easter Eggs" OFF)
option(PURPL_USE_SYSTEM_SDL "Use the system installation of SDL" OFF)
option(PURPL_ENABLE_DISCORD "Enable Discord integration" OFF)
option(PURPL_ENABLE_STEAM "Enable Steam integration" OFF)

if (${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore" AND ${CMAKE_BUILD_TYPE} MATCHES "Deb")
	set(_MSIX_VCLIBS_EXTRA .Debug)
endif()

# Dependencies
if (NOT APPLE)
	add_subdirectory(deps/glad/cmake glad_cmake)
endif()
if (${PURPL_USE_SYSTEM_SDL})
	find_package(SDL2)
endif()

if (${PURPL_ENABLE_DISCORD} AND NOT ${PURPL_PROCESSOR} MATCHES "x86")
	message(WARNING "Discord only supports x86, disabling Discord integration")
	set(PURPL_ENABLE_DISCORD OFF)
endif()
if (${PURPL_ENABLE_DISCORD} AND NOT ${PURPL_PROCESSOR} STREQUAL "x86_64")
	message(WARNING "Steam only supports x86, disabling Steam integration")
	set(PURPL_ENABLE_STEAM OFF)
endif()

if (NOT ${PURPL_USE_SYSTEM_SDL} OR NOT ${SDL2_FOUND})
	message(STATUS "Using bundled SDL")
	set(SDL_STATIC OFF CACHE BOOL "" FORCE)
	set(SDL_CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
	add_subdirectory(deps/sdl2 EXCLUDE_FROM_ALL)
	if (DEFINED DEFAULT_C_LIBRARIES)
		target_link_libraries(SDL2 PRIVATE ${EXTRA_LIBS})
	endif()
	set(SDL2_LIBRARIES SDL2::SDL2)
	set(SDL2_INCLUDE_DIRS deps/sdl2/include)
	set(SDL2_FOUND FALSE CACHE BOOL "" FORCE)

	install(TARGETS SDL2
		LIBRARY
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	if (MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Deb")
		install(PROGRAMS $<TARGET_PDB_FILE:SDL2>
			DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	endif()
endif()

set(SPIRV_CROSS_SKIP_INSTALL ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/spirv-cross)
set(SPIRV_CROSS_EXECUTABLE $<TARGET_FILE:spirv-cross>)

if (${PURPL_ENABLE_DISCORD})
	if (NOT DEFINED PURPL_DISCORD_APP_ID)
		message(FATAL_ERROR "PURPL_DISCORD_APP_ID must be set if PURPL_ENABLE_DISCORD is true")
	endif()
	get_discord_sdk(2.5.6)
endif()
if (${PURPL_ENABLE_STEAM})
	get_steamworks_sdk()
endif()

if (${CMAKE_C_COMPILER_ID} MATCHES "CLANG")
	set(CMAKE_C_STANDARD 2x)
else()
	set(CMAKE_C_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD 20)
if (NOT MSVC)
	# C17 support in Visual Studio is relatively recent
	set(CMAKE_C_STANDARD_REQUIRED TRUE)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Graphics APIs
if (APPLE)
	set(Vulkan_GLSLC_EXECUTABLE ${CMAKE_SOURCE_DIR}/tools/glslc)
else()
	find_package(Vulkan REQUIRED)

	if ("${Vulkan_GLSLC_EXECUTABLE}" STREQUAL "Vulkan_GLSLC_EXECUTABLE-NOTFOUND")
		set(Vulkan_GLSLC_EXECUTABLE ${CMAKE_SOURCE_DIR}/tools/glslc)
	endif()

	glad_add_library(glad_opengl REPRODUCIBLE LANGUAGE c LOADER API gl:core=4.6)
	glad_add_library(glad_vulkan REPRODUCIBLE LANGUAGE c LOADER API vulkan=1.3)
endif()

include(cmake/Shader.cmake)

find_package(Python3 REQUIRED)

get_git_head_revision(_, ENGINE_COMMIT)
if (${CMAKE_BUILD_TYPE} MATCHES "Rel")
	string(SUBSTRING "${ENGINE_COMMIT}" 0 7 ENGINE_COMMIT)
endif()
git_branch_name(ENGINE_BRANCH)
if (${ENGINE_BRANCH} STREQUAL "-128-NOTFOUND")
	set(ENGINE_BRANCH "unknown")
endif()

message(STATUS "")
message(STATUS "Purpl Engine configuration: ")
message(STATUS "")
message(STATUS "\tPlatform: ${CMAKE_SYSTEM} (pointer size: ${CMAKE_SIZEOF_VOID_P})")
message(STATUS "\tProcessor:")
message(STATUS "\t\tHost: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "\t\tTarget: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "\tGenerator: ${CMAKE_GENERATOR}")
message(STATUS "\tBuild type: ${CMAKE_BUILD_TYPE}")
message(STATUS "\tRevision: ${ENGINE_BRANCH} commit ${ENGINE_COMMIT}")
message(STATUS "")
if (WIN32)
	message(STATUS "\tAssembler: ${CMAKE_ASM_MASM_COMPILER}")
else()
	message(STATUS "\tAssembler: ${CMAKE_ASM_COMPILER}")
endif()
foreach (lang C CXX)
	if ("${lang}" MATCHES "CXX")
		set(lang_name C++)
	else()
		set(lang_name ${lang})
	endif()
	message(STATUS "\t${lang_name} compiler: ${CMAKE_C_COMPILER}")
	if (${CMAKE_${lang}_COMPILER_ID} MATCHES "Clang" OR NOT MSVC)
		execute_process(COMMAND ${CMAKE_${lang}_COMPILER} --version OUTPUT_VARIABLE ${lang}_COMPILER_VERSION)
	else()
		# MSVC always outputs version information unless /nologo is specified
		execute_process(COMMAND ${CMAKE_${lang}_COMPILER} ERROR_VARIABLE ${lang}_COMPILER_VERSION OUTPUT_VARIABLE _)
	endif()
	string(REGEX REPLACE ";" "\\\\;" ${lang}_COMPILER_VERSION "${${lang}_COMPILER_VERSION}")
	string(REGEX REPLACE "\n" ";" ${lang}_COMPILER_VERSION "${${lang}_COMPILER_VERSION}")
	message(STATUS "\t${lang_name} compiler information:")
	foreach (X ${${lang}_COMPILER_VERSION})
		message(STATUS "\t\t${X}")
	endforeach()
	message(STATUS "\t${lang_name} flags: ${CMAKE_${lang}_FLAGS}")
endforeach()
message(STATUS "\tGLSL compiler: ${Vulkan_GLSLC_EXECUTABLE}")
message(STATUS "\tSPIRV-Cross: ${SPIRV_CROSS_EXECUTABLE}")
if (APPLE)
	message(STATUS "\tSwift compiler: ${CMAKE_Swift_COMPILER}")
endif()
message(STATUS "")
message(STATUS "\tPython: ${Python3_EXECUTABLE}")
message(STATUS "")
message(STATUS "\tCMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
if (${CMAKE_GENERATOR} MATCHES "Makefiles" OR ${CMAKE_GENERATOR} MATCHES "Ninja")
	message(STATUS "\tCMAKE_EXPORT_COMPILE_COMMANDS: ${CMAKE_EXPORT_COMPILE_COMMANDS}")
else()
	message(STATUS "\tCMAKE_EXPORT_COMPILE_COMMANDS: unsupported, use Ninja or Makefiles")
endif()
message(STATUS "\tEnable demo: ${PURPL_ENABLE_DEMO}")
message(STATUS "\tEnable Easter Eggs: ${PURPL_ENABLE_MEMING}")
message(STATUS "\tDiscord integration: ${PURPL_ENABLE_DISCORD}")
if (${PURPL_ENABLE_DISCORD})
	message(STATUS "\t\tDiscord app ID: ${PURPL_DISCORD_APP_ID}")
endif()
message(STATUS "\tSteam integration: ${PURPL_ENABLE_STEAM}")
message(STATUS "")
if ("$ENV{VULKAN_SDK}" STREQUAL "")
	message(STATUS "\tVulkan SDK: not defined, libraries likely in standard location")
else()
	message(STATUS "\tVulkan SDK: $ENV{VULKAN_SDK}")
endif()
message(STATUS "")
if (${SDL2_FOUND})
	message(STATUS "\tSDL: system")
else()
	message(STATUS "\tSDL: bundled")
endif()
message(STATUS "\tVulkan libraries: ${Vulkan_LIBRARIES}")
message(STATUS "")

# Add the engine and the demo if it's enabled
add_subdirectory(engine)
if (${PURPL_ENABLE_DEMO})
	add_subdirectory(demo)
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT purpl-demo)
endif()

# Group together files
add_custom_target(files ALL SOURCES
				cmake/GetGitRevisionDescription.cmake
				cmake/Platform.cmake
				cmake/SDKs.cmake
				cmake/Shader.cmake
				README.md
				LICENSE
				LICENSE_TEMPLATE.txt)

# Install targets for dependencies
foreach (target ${ENGINE_RUNTIME_TARGETS})
	get_target_property(target_imported ${target} IMPORTED)
	if (${target_imported})
		install(PROGRAMS $<TARGET_FILE:${target}>
			DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	else()
		install(TARGETS ${target}
			LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
		if (WIN32 AND MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Deb")
			install(PROGRAMS $<TARGET_PDB_FILE:${target}>
				DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
		endif()
	endif()
endforeach()

# Install licenses
install(FILES deps/glad/LICENSE
	RENAME glad.txt
	DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses)
install(FILES deps/sdl2/LICENSE.txt
	RENAME sdl2.txt
	DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses)
install(FILES deps/stb/LICENSE
	RENAME stb.txt
	DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses)
install(FILES LICENSE
	RENAME purpl.txt
	DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses)

