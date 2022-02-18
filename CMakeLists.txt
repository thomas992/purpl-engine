cmake_minimum_required(VERSION 3.10)

# Use this if Ninja can't be found
if (${CMAKE_GENERATOR} MATCHES "Ninja")
	if (WIN32)
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja.exe CACHE STRING "" FORCE)
	else()
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja CACHE STRING "" FORCE)
	endif()
endif()

# Declare the project
project(purpl-engine VERSION 1.0.0 LANGUAGES C CXX)

# Linux Mint's GCC's C2x support doesn't seem have [[fallthrough]] yet
if (${CMAKE_C_COMPILER_ID} MATCHES "GNU")
	message(WARNING "GCC may not be able to compile SDL2")
endif()

# Settings
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
option(PURPL_ENABLE_DEMO "Build the demo" ON)
option(PURPL_ENABLE_MEMING "Easter Eggs" OFF)

# Dependencies
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
add_subdirectory(deps/sdl2 EXCLUDE_FROM_ALL)

# Compiler and linker flags
if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c17 /W4 /WX /wd4005 /wd4057 /wd4090 /wd4100 /wd4115 /wd4133 /wd4220 /wd4244 /wd4267 /wd4324 /wd4342 /wd4706 /wd4996")
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
endif()

# Install licenses
install(DIRECTORY deps/licenses
	DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES LICENSE
	RENAME purpl.txt
	DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses)

# Install select files from dependencies
install(TARGETS SDL2
	LIBRARY
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# Install the run script
if (WIN32)
	install(FILES tools/run.bat
		DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
	install(FILES tools/run.sh
		DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
