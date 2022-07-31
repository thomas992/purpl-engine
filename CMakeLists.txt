cmake_minimum_required(VERSION 3.22)

include(CheckIPOSupported)
include(cmake/GetGitRevisionDescription.cmake)
include(cmake/Util.cmake)

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

# Set the build type
if (NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# PIC
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set the install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/game)

# System detection
if (NOT DEFINED CMAKE_SYSTEM_PROCESSOR AND NOT DEFINED CMAKE_HOST_SYSTEM_PROCESSOR)
	message(FATAL_ERROR "Missing processor name")
elseif (NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
	set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
elseif (NOT DEFINED CMAKE_HOST_SYSTEM_PROCESSOR)
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
message(STATUS "Host processor is ${PURPL_HOST_PROCESSOR} (determined from ${CMAKE_HOST_SYSTEM_PROCESSOR})")

if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR
    "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86-64")
	set(PURPL_TARGET_PROCESSOR "x64")
elseif ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "ARM64" OR
	"${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64" OR
	"${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv8-a")
	set(PURPL_TARGET_PROCESSOR "ARM64")
endif()
message(STATUS "Target processor is ${PURPL_TARGET_PROCESSOR} (determined from ${CMAKE_SYSTEM_PROCESSOR})")

# Dependencies
add_subdirectory(deps/flecs EXCLUDE_FROM_ALL)

# SDL needs some stuff to use the GDK from CMake
if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES "Gaming")
	add_compile_definitions(HAVE_STDINT_H=1) # SDL gets confused about stdint types
endif()
add_subdirectory(deps/sdl2 EXCLUDE_FROM_ALL)
if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES "Gaming")
	target_sources(SDL2 PRIVATE deps/sdl2/src/core/gdk/SDL_gdk.cpp)
	target_sources(SDL2main PRIVATE deps/sdl2/src/main/gdk/SDL_gdk_main.c)
	target_link_libraries(SDL2 PRIVATE libHttpClient.142.GDK.C.lib Microsoft.Xbox.Services.142.GDK.C.lib XCurl.lib xgameruntime.lib appnotify.lib gameinput.lib winhttp.lib crypt32.lib)
	if ("${CMAKE_BUILD_TYPE}" MATCHES "Deb")
		target_link_libraries(SDL2 PRIVATE msvcprtd.lib msvcrtd.lib ucrtd.lib vcruntimed.lib)
	else()
		target_link_libraries(SDL2 PRIVATE msvcprt.lib msvcrt.lib ucrt.lib vcruntime.lib)
	endif()
	set_property(TARGET SDL2 PROPERTY VS_GLOBAL_GDKExtLibNames Xbox.Services.API.C Xbox.XCurl.API)
	target_link_options(SDL2 PRIVATE /alternatename:__imp___std_init_once_complete=__imp_InitOnceComplete /alternatename:__imp___std_init_once_begin_initialize=__imp_InitOnceBeginInitialize)
endif()

add_subdirectory(deps/zstd/build/cmake EXCLUDE_FROM_ALL)

# Include directories
set(PURPL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/deps/cglm/include
		       ${CMAKE_SOURCE_DIR}/deps/flecs
		       ${CMAKE_SOURCE_DIR}/deps/sdl2/include
		       ${CMAKE_SOURCE_DIR}/deps/stb
		       ${CMAKE_SOURCE_DIR}/deps/zstd/lib
		       ${CMAKE_SOURCE_DIR})

if ("${CMAKE_BUILD_TYPE}" MATCHES "Deb")
	add_compile_definitions(PURPL_DEBUG=1)
elseif ("${CMAKE_BUILD_TYPE}" MATCHES "Rel")
	add_compile_definitions(PURPL_RELEASE=1)
endif()

# LTO
check_ipo_supported(RESULT CMAKE_INTERPROCEDURAL_OPTIMIZATION OUTPUT error)

# Git revision
git_describe(PURPL_GIT_DESC)

# Subdirectories
add_subdirectory(common)
add_subdirectory(engine)
add_subdirectory(launcher)
add_subdirectory(tools/paktool)

# Set the startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT launcher)

# Install files into a layout similar to Source 2 because Source 2 is cool
install(TARGETS flecs
		SDL2
		libzstd_shared

		engine
		launcher

		paktool
	RUNTIME DESTINATION bin)

if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES "Gaming")
	set(PURPL_GAME_XBOX_TARGET ${CMAKE_VS_PLATFORM_NAME})
	string(REPLACE "Gaming" "" PURPL_GAME_XBOX_TARGET "${PURPL_GAME_XBOX_TARGET}")
	string(REPLACE ".Xbox." "" PURPL_GAME_XBOX_TARGET "${PURPL_GAME_XBOX_TARGET}")
	string(REPLACE ".x64" "" PURPL_GAME_XBOX_TARGET "${PURPL_GAME_XBOX_TARGET}")
	string(REPLACE ".Desktop" "PC" PURPL_GAME_XBOX_TARGET "${PURPL_GAME_XBOX_TARGET}")
	configure_file(${CMAKE_BINARY_DIR}/MicrosoftGame.Config ${CMAKE_BINARY_DIR}/configured/MicrosoftGame.Config)
	install(FILES ${CMAKE_BINARY_DIR}/configured/MicrosoftGame.Config
		      ${CMAKE_SOURCE_DIR}/launcher/Assets/StoreLogo.png
		      ${CMAKE_SOURCE_DIR}/launcher/Assets/Logo.png
		      ${CMAKE_SOURCE_DIR}/launcher/Assets/SmallLogo.png
		      ${CMAKE_SOURCE_DIR}/launcher/Assets/SplashScreen.png
		      ${CMAKE_SOURCE_DIR}/launcher/Assets/LargeLogo.png
		DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

# Collect the licenses of the dependencies into one file like Source engine does
string(REPEAT "*" 84 PURPL_LICENSE_SEPARATOR)
set(PURPL_LICENSES_FILE ${CMAKE_BINARY_DIR}/thirdpartylegalnotices.txt)
set(PURPL_LICENSES "CGLM (https://github.com/recp/cglm)" ${CMAKE_SOURCE_DIR}/deps/cglm/LICENSE
		   "dirent.h for Windows (https://github.com/tronkko/dirent)" ${CMAKE_SOURCE_DIR}/common/_dirent.h
		   "Flecs (https://github.com/SanderMertens/flecs" ${CMAKE_SOURCE_DIR}/deps/flecs/LICENSE
		   "SDL2 (https://libsdl.org)" ${CMAKE_SOURCE_DIR}/deps/sdl2/LICENSE.txt
		   "stb (https://github.com/nothings/stb)" ${CMAKE_SOURCE_DIR}/deps/stb/LICENSE)
file(WRITE ${PURPL_LICENSES_FILE} [=[
I'm too lazy to write the code that these libraries provide and they're open
source so here's the licenses.

NOTE: you're gonna wanna use Ctrl-F or grep or / or something to find stuff
in here rather than scrolling because some licenses are just straight up C
header files because I couldn't be bothered to do anything fancier with my
janky CMake script to generate this file.
]=])
file(APPEND ${PURPL_LICENSES_FILE} "\n${PURPL_LICENSE_SEPARATOR}\n")
foreach (license ${PURPL_LICENSES})
	if ("${license}" MATCHES "${CMAKE_SOURCE_DIR}")
		file(READ ${license} license_text)
		file(APPEND ${PURPL_LICENSES_FILE} ${license_text})
		file(APPEND ${PURPL_LICENSES_FILE} "\n${PURPL_LICENSE_SEPARATOR}\n")
	else()
		file(APPEND ${PURPL_LICENSES_FILE} "${license}\n")
	endif()
endforeach()
install(FILES ${PURPL_LICENSES_FILE}
	DESTINATION ${CMAKE_INSTALL_PREFIX})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/game/data/core/
	DESTINATION core)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/game/data/purpl/
	DESTINATION purpl)
