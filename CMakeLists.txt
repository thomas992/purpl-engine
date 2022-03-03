cmake_minimum_required(VERSION 3.10)

include(cmake/GetGitRevisionDescription.cmake)

# Use the engine's Ninja
if (${CMAKE_GENERATOR} MATCHES "Ninja")
	if (WIN32)
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja.exe CACHE STRING "" FORCE)
	else()
		set(CMAKE_MAKE_PROGRAM ${CMAKE_SOURCE_DIR}/tools/ninja CACHE STRING "" FORCE)
	endif()
endif()

# Dependencies (has to go before the custom rules but after the fix for Ninja and Windows Clang)
if (NOT APPLE)
	add_subdirectory(deps/glad/cmake glad_cmake)
endif()
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
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
option(PURPL_ENABLE_DEMO "Build the demo" ON)
option(PURPL_ENABLE_MEMING "Easter Eggs" OFF)

# Compiler and linker flags
if (MSVC)
	string(REGEX REPLACE "/W[1-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c17 /W4 /WX")
	if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations -Wno-ignored-qualifiers -Wno-incompatible-pointer-types-discards-qualifiers -Wno-microsoft-enum-forward-reference -Wno-pointer-sign -Wno-pragma-pack -Wno-sign-compare -Wno-unused-parameter -Wno-visibility")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4005 /wd4057 /wd4090 /wd4100 /wd4115 /wd4133 /wd4152 /wd4220 /wd4244 /wd4267 /wd4324 /wd4335 /wd4342 /wd4701 /wd4703 /wd4706 /wd4996 /wd5105")
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
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations -Wno-ignored-qualifiers -Wno-incompatible-pointer-types-discards-qualifiers -Wno-microsoft-enum-forward-reference -Wno-pointer-sign -Wno-pragma-pack -Wno-sign-compare -Wno-unused-parameter -Wno-visibility")
	else() # Assume GCC warning names
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-discarded-qualifiers -Wno-pointer-sign -Wno-sign-compare -Wno-unused-parameter")
	endif()
endif()

# Vulkan SDK
if (NOT APPLE)
	cmake_policy(SET CMP0072 NEW)
	find_package(OpenGL REQUIRED)
	find_package(Vulkan REQUIRED)

	glad_add_library(glad_opengl REPRODUCIBLE API gl:core=4.6)
	glad_add_library(glad_vulkan REPRODUCIBLE API vulkan=1.3)
endif()

get_git_head_revision(_, ENGINE_COMMIT)
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
message(STATUS "\tCMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "\tEnable demo: ${PURPL_ENABLE_DEMO}")
message(STATUS "\tEnable Easter Eggs: ${PURPL_ENABLE_MEMING}")
message(STATUS "")
message(STATUS "\tVulkan SDK: $ENV{VULKAN_SDK}")
message(STATUS "")
message(STATUS "\tOpenGL libraries: ${OPENGL_LIBRARIES}")
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
