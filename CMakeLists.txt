cmake_minimum_required(VERSION 3.10)

# Use the engine's Ninja
if (${CMAKE_GENERATOR} MATCHES "Ninja")
	if (WIN32)
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja.exe CACHE STRING "" FORCE)
	else()
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja CACHE STRING "" FORCE)
	endif()
endif()

# I hate GCC, and it's slower and unable to compile SDL
if (NOT WIN32)
	set(PURPL_USE_LLVM ON)
endif()

if(${PURPL_USE_LLVM})
	message(STATUS "Using LLVM to build")
	if (WIN32)
		set(CMAKE_C_COMPILER clang-cl)
		set(CMAKE_CXX_COMPILER clang-cl)
		set(CMAKE_C_FLAGS "-fuse-ld=lld-link")
		set(CMAKE_CXX_FLAGS "-fuse-ld=lld-link")
	else()
		set(CMAKE_C_COMPILER clang)
		set(CMAKE_CXX_COMPILER clang++)
		set(CMAKE_C_FLAGS "-fuse-ld=lld")
		set(CMAKE_CXX_FLAGS "-fuse-ld=lld")
	endif()
endif()

# Dependencies (has to go before the custom rules but after the fix for Ninja)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
add_subdirectory(deps/sdl2 EXCLUDE_FROM_ALL)

# Don't link to anything
file(WRITE ${CMAKE_BINARY_DIR}/MakeRulesOverwrite.cmake
[=[
if (MSVC)
	set(CMAKE_C_STANDARD_LIBRARIES_INIT "kernel32.lib")
	set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "kernel32.lib")
endif()
]=]
)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_BINARY_DIR}/MakeRulesOverwrite.cmake")

# Declare the project
project(purpl-engine VERSION 1.0.0 LANGUAGES C CXX)

# Settings
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
option(PURPL_ENABLE_DEMO "Build the demo" ON)
option(PURPL_ENABLE_MEMING "Easter Eggs" OFF)

# Compiler and linker flags
if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c17 /W4 /WX /wd4005 /wd4057 /wd4090 /wd4100 /wd4115 /wd4133 /wd4220 /wd4244 /wd4267 /wd4324 /wd4342 /wd4701 /wd4703 /wd4706 /wd4996")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu2x -Wall -Wextra -Werror")
	if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-ignored-qualifiers -Wno-incompatible-pointer-types-discards-qualifiers -Wno-pointer-sign -Wno-sign-compare -Wno-unused-parameter")
	else() # Assume GCC warning names
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-discarded-qualifiers -Wno-pointer-sign -Wno-sign-compare -Wno-unused-parameter")
	endif()
endif()

# Add the engine and the demo if it's enabled
add_subdirectory(engine)
if (${PURPL_ENABLE_DEMO})
	add_subdirectory(demo)
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT demo)
endif()

# Install licenses
install(FILES deps/phnt/LICENSE
	RENAME phnt.txt
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

# Install select files from dependencies
install(TARGETS SDL2
	LIBRARY
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# Install the run script
if (WIN32)
	install(PROGRAMS tools/run.bat
		DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
	install(PROGRAMS tools/run.sh
		DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
