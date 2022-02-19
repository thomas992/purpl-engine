cmake_minimum_required(VERSION 3.10)

set(ENGINE_CORE_HEADERS
	include/purpl/ENGINE.h

	include/purpl/core/coredefs.h
	include/purpl/core/features.h
	include/purpl/core/init.h
	include/purpl/core/inst.h
	include/purpl/core/log.h
	include/purpl/core/types.h
)

set(ENGINE_GRAPHICS_HEADERS
	include/graphics/init.h
)

set(ENGINE_UTIL_HEADERS
	include/purpl/util/error.h
	include/purpl/util/fs.h
	include/purpl/util/misc.h
	include/purpl/util/string.h
	include/purpl/util/util.h
)

set(ENGINE_WINDOWS_HEADERS
	include/purpl/util/private/win/ntdll.h
)

set(ENGINE_CORE_SOURCES
	src/core/globals.c
	src/core/init.c
	src/core/log.c
)

set(ENGINE_UTIL_SOURCES
	src/util/error.c
	src/util/fs.c
	src/util/misc.c
	src/util/stb.c
	src/util/string.c
)

set(ENGINE_WINDOWS_SOURCES
	src/util/win/ntdll.c
)

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
	${ENGINE_CORE_HEADERS}
	${ENGINE_GRAPHICS_HEADERS}
	${ENGINE_UTIL_HEADERS}
	${ENGINE_PLATFORM_HEADERS}
)

set(ENGINE_ALL_SOURCES
	${ENGINE_CORE_SOURCES}
	${ENGINE_GRAPHICS_SOURCES}
	${ENGINE_UTIL_SOURCES}
	${ENGINE_PLATFORM_SOURCES}
)

add_library(engine SHARED ${ENGINE_ALL_SOURCES})
target_compile_definitions(engine PRIVATE PURPL_BUILD=1)
if (${PURPL_ENABLE_MEMING})
	target_compile_definitions(engine PRIVATE PURPL_ENABLE_MEMING=1)
endif()
if (${CMAKE_BUILD_TYPE} EQUALS "Debug")
	target_compile_definitions(engine PRIVATE PURPL_DEBUG=1)
endif()
target_include_directories(engine PUBLIC
					${CMAKE_CURRENT_SOURCE_DIR}/include
				PRIVATE
					${CMAKE_SOURCE_DIR}/deps/phnt
					${CMAKE_SOURCE_DIR}/deps/sdl2/include
					${CMAKE_SOURCE_DIR}/deps/stb)
target_link_libraries(engine PRIVATE SDL2::SDL2)

install(TARGETS engine
	LIBRARY
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
