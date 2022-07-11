cmake_minimum_required(VERSION 3.23)

# Declare the project
project(purpl VERSION 1.0.0
	      DESCRIPTION "The Purpl game engine"
	      LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS TRUE)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Prevent in-tree builds
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "In-tree builds not allowed. Run CMake from a separate directory or use the -B parameter")
endif()

# Set the install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/game)

# System detection
if (NOT DEFINED CMAKE_HOST_SYSTEM_PROCESSOR)
	set(CMAKE_HOST_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
endif()
if ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR
    "${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "x86-64")
	set(PURPL_HOST_PROCESSOR "x64")
elseif ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "ARM64" OR
	"${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "aarch64" OR
	"${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "armv8-a")
	set(PURPL_HOST_PROCESSOR "ARM64")
endif()
message(STATUS "Host processor is ${PURPL_HOST_PROCESSOR}")

if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR
    "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86-64")
	set(PURPL_TARGET_PROCESSOR "x64")
elseif ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "ARM64" OR
	"${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64" OR
	"${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv8-a")
	set(PURPL_TARGET_PROCESSOR "ARM64")
endif()
message(STATUS "Target processor is ${PURPL_TARGET_PROCESSOR}")

# Dependencies
add_subdirectory(deps/flecs EXCLUDE_FROM_ALL)
add_subdirectory(deps/sdl2 EXCLUDE_FROM_ALL)
add_subdirectory(deps/zstd/build/cmake EXCLUDE_FROM_ALL)

# Include directories
set(PURPL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/deps/cglm/include
		       ${CMAKE_SOURCE_DIR}/deps/flecs
		       ${CMAKE_SOURCE_DIR}/deps/sdl2/include
		       ${CMAKE_SOURCE_DIR}/deps/stb
		       ${CMAKE_SOURCE_DIR}/deps/zstd/lib
		       ${CMAKE_SOURCE_DIR})

# Subdirectories
add_subdirectory(common)
add_subdirectory(engine)
add_subdirectory(launcher)
add_subdirectory(tools/paktool)

# Install files into a layout similar to Source 2 because Source 2 is cool
install(TARGETS flecs
		SDL2
		libzstd_shared

		engine
		launcher

		paktool
	RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/deps/license/
	DESTINATION licenses)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/game/data/core/
	DESTINATION core)

