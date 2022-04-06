cmake_minimum_required(VERSION 3.15)

include(cmake/GetGitRevisionDescription.cmake)

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
if (MSVC)
	set(DEFAULT_C_LIBRARIES ${CMAKE_C_STANDARD_LIBRARIES_INIT})
	set(DEFAULT_CXX_LIBRARIES ${CMAKE_C_STANDARD_LIBRARIES_INIT})
	set(CMAKE_C_STANDARD_LIBRARIES_INIT "kernel32.lib ntdll.lib user32.lib")
	set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "kernel32.lib ntdll.lib user32.lib")
endif()
]=]
)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_BINARY_DIR}/MakeRulesOverwrite.cmake")

# Declare the project
project(purpl-engine VERSION 1.0.0 LANGUAGES C CXX)

if (WIN32)
	enable_language(ASM_MASM)
else()
	set(CMAKE_ASM_NASM_COMPILER ${CMAKE_SOURCE_DIR}/tools/nasm CACHE STRING "" FORCE)
	set(CMAKE_ASM_NASM_LINK_EXECUTABLE "ld <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>" CACHE STRING "" FORCE)
	set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS S CACHE STRING "" FORCE)
	if (APPLE)
		set(CMAKE_ASM_NASM_OBJECT_FORMAT macho64 CACHE STRING "" FORCE)
	else()
		set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64 CACHE STRING "" FORCE)
	endif()
	set(CMAKE_ASM_NASM_FLAGS_DEBUG "-gdwarf" CACHE STRING "" FORCE)
	enable_language(ASM_NASM)
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

# Dependencies
if (NOT APPLE)
	add_subdirectory(deps/glad/cmake glad_cmake)
endif()
if (PURPL_USE_SYSTEM_SDL)
	find_package(SDL2)
endif()

if (NOT PURPL_USE_SYSTEM_SDL OR NOT SDL2_FOUND)
	message(STATUS "Using bundled SDL")
	set(SDL_STATIC OFF CACHE BOOL "" FORCE)
	set(SDL_CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
	add_subdirectory(deps/sdl2 EXCLUDE_FROM_ALL)
	if (DEFINED DEFAULT_C_LIBRARIES)
		target_link_libraries(SDL2 PRIVATE ${DEFAULT_C_LIBRARIES})
	endif()
	set(SDL2_LIBRARIES SDL2::SDL2)
	set(SDL2_INCLUDE_DIRS deps/sdl2/include)
endif()

# Compiler and linker flags
if (MSVC)
	string(REGEX REPLACE "/W[1-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c17 /W4 /WX")
	if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations -Wno-gnu-folding-constant -Wno-microsoft-enum-forward-reference -Wno-pragma-pack -Wno-sign-compare -Wno-visibility")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4115 /wd4244 /wd4267 /wd4324 /wd4335 /wd4996 /wd5105")
	endif()
else()
	if (${CMAKE_C_COMPILER_ID} MATCHES "GNU")
		# GCC 9's C2x support doesn't have [[fallthrough]], but SDL assumes it does
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu17")
	else()
		# Clang has no issues
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu2x")
	endif()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
	if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations -Wno-gnu-folding-constant -Wno-incompatible-pointer-types-discards-qualifiers -Wno-microsoft-enum-forward-reference -Wno-pragma-pack -Wno-sign-compare -Wno-visibility")
	else() # Assume GCC warning names
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-discarded-qualifiers -Wno-incompatible-pointer-types -Wno-pointer-sign -Wno-sign-compare -Wno-unused-parameter")
	endif()
endif()

# Graphics APIs
if (NOT APPLE)
	find_package(Vulkan REQUIRED)

	glad_add_library(glad_opengl REPRODUCIBLE LANGUAGE c LOADER API gl:core=4.6)
	glad_add_library(glad_vulkan REPRODUCIBLE LANGUAGE c LOADER API vulkan=1.3)
endif()

find_package(Python3 REQUIRED)

get_git_head_revision(_, ENGINE_COMMIT)
if (${CMAKE_BUILD_TYPE} MATCHES "Rel")
	string(SUBSTRING "${ENGINE_COMMIT}" 0 7 ENGINE_COMMIT)
endif()
git_branch_name(ENGINE_BRANCH)

message(STATUS "")
message(STATUS "Purpl Engine configuration: ")
message(STATUS "")
message(STATUS "\tPlatform: ${CMAKE_SYSTEM} (pointer size: ${CMAKE_SIZEOF_VOID_P})")
message(STATUS "\tBuild type: ${CMAKE_BUILD_TYPE}")
message(STATUS "\tRevision: ${ENGINE_BRANCH} commit ${ENGINE_COMMIT}")
message(STATUS "")
message(STATUS "\tCC: ${CMAKE_C_COMPILER}")
message(STATUS "\tCFLAGS: ${CMAKE_C_FLAGS}")
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
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT demo)
endif()

# Install licenses
install(FILES deps/glad/LICENSE
	RENAME glad.txt
	DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses)
install(FILES deps/processhacker/LICENSE.txt
	RENAME processhacker.txt
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
if (NOT PURPL_USE_SYSTEM_SDL)
	install(TARGETS SDL2
		LIBRARY
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

if (MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Deb")
	install(PROGRAMS $<TARGET_PDB_FILE:SDL2>
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()
