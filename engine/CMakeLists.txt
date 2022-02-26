cmake_minimum_required(VERSION 3.10)

set(ENGINE_CORE_HEADERS
    include/purpl/core/coredefs.h
    include/purpl/core/features.h
    include/purpl/core/init.h
    include/purpl/core/inst.h
    include/purpl/core/log.h
    include/purpl/core/types.h)

set(ENGINE_GRAPHICS_HEADERS
    include/purpl/graphics/init.h)

set(ENGINE_VULKAN_HEADERS
    include/purpl/graphics/vulkan/init.h
    include/purpl/graphics/vulkan/inst.h)

set(ENGINE_UTIL_HEADERS
    include/purpl/util/error.h
    include/purpl/util/fs.h
    include/purpl/util/misc.h
    include/purpl/util/string.h
    include/purpl/util/util.h)

set(ENGINE_CORE_SOURCES
    src/core/globals.c
    src/core/init.c
    src/core/log.c)

set(ENGINE_GRAPHICS_SOURCES
    src/graphics/init.c)

set(ENGINE_VULKAN_SOURCES
    src/graphics/vulkan/init.c
    src/graphics/vulkan/inst.c)

set(ENGINE_UTIL_SOURCES
    src/util/error.c
    src/util/fs.c
    src/util/misc.c
    src/util/stb.c
    src/util/string.c)

set(ENGINE_WINDOWS_SOURCES src/engine.rc)

if (NOT APPLE)
	set(ENGINE_GRAPHICS_HEADERS ${ENGINE_GRAPHICS_HEADERS} ${ENGINE_VULKAN_HEADERS})
	set(ENGINE_GRAPHICS_SOURCES ${ENGINE_GRAPHICS_SOURCES} ${ENGINE_VULKAN_SOURCES})
endif()


if (WIN32)
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_WINDOWS_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_WINDOWS_SOURCES})
elseif (APPLE)
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_MACOS_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_MACOS_SOURCES})
else()
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_LINUX_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_LINUX_SOURCES})
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

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${ENGINE_ALL_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${ENGINE_ALL_SOURCES})

add_library(engine SHARED ${ENGINE_ALL_HEADERS} ${ENGINE_ALL_SOURCES})
target_compile_definitions(engine PRIVATE PURPL_BUILD=1)

if (${PURPL_ENABLE_MEMING})
	target_compile_definitions(engine PRIVATE PURPL_ENABLE_MEMING=1)
endif()
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	target_compile_definitions(engine PRIVATE PURPL_DEBUG=1)
endif()
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	target_compile_definitions(engine PRIVATE PURPL_X86=1 PURPL_X86_64=1)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "86")
	target_compile_definitions(engine PRIVATE PURPL_X86=1)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "a.*64")
	target_compile_definitions(engine PRIVATE PURPL_ARM=1)
endif()

target_include_directories(engine PUBLIC
			   ${CMAKE_CURRENT_SOURCE_DIR}/include
			   PRIVATE
			   ${Vulkan_INCLUDE_DIRS}
			   ${CMAKE_SOURCE_DIR}/deps/phnt
			   ${CMAKE_SOURCE_DIR}/deps/sdl2/include
					${CMAKE_SOURCE_DIR}/deps/stb)
target_link_libraries(engine PRIVATE SDL2::SDL2 ${Vulkan_LIBRARIES})

if (WIN32)
	set(ENGINE_RUNTIME_FILES $<TARGET_FILE:engine> $<TARGET_PDB_FILE:engine> $<TARGET_FILE:SDL2::SDL2> $<TARGET_PDB_FILE:SDL2::SDL2> PARENT_SCOPE)
endif()

install(TARGETS engine
	LIBRARY
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
