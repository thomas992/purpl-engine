cmake_minimum_required(VERSION 3.10)

set(ENGINE_CORE_HEADERS
    include/purpl/core/coredefs.h
    include/purpl/core/features.h
    include/purpl/core/init.h
    include/purpl/core/inst.h
    include/purpl/core/log.h
    include/purpl/core/types.h)

set(ENGINE_GRAPHICS_HEADERS
    include/purpl/graphics/init.h
    include/purpl/graphics/inst_data.h
    include/purpl/graphics/window.h)

set(ENGINE_VULKAN_HEADERS
    include/purpl/graphics/vulkan/debug.h
    include/purpl/graphics/vulkan/device.h
    include/purpl/graphics/vulkan/init.h
    include/purpl/graphics/vulkan/inst_data.h
    include/purpl/graphics/vulkan/inst.h
    include/purpl/graphics/vulkan/swapchain.h)

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

set(ENGINE_DEBUG_SOURCES)

set(ENGINE_GRAPHICS_SOURCES
    src/graphics/init.c
    src/graphics/window.c)

set(ENGINE_SHADERS
    src/graphics/shaders/triangle.vert
    src/graphics/shaders/triangle.frag)

set(ENGINE_VULKAN_SOURCES
    src/graphics/vulkan/debug.c
    src/graphics/vulkan/device.c
    src/graphics/vulkan/init.c
    src/graphics/vulkan/inst.c
    src/graphics/vulkan/swapchain.c)

set(ENGINE_METAL_SOURCES
    src/graphics/metal/init.swift)

set(ENGINE_UTIL_SOURCES
    src/util/error.c
    src/util/fs.c
    src/util/misc.c
    src/util/stb.c
    src/util/string.c)

set(ENGINE_WINDOWS_AMD64_SOURCES
    src/util/x86/random_64.asm)

set(ENGINE_MACOS_x86_64_SOURCES
    src/util/x86/random_64_mac.S)

set(ENGINE_LINUX_x86_64_SOURCES
    src/util/x86/random_64.S)

set(ENGINE_WINDOWS_SOURCES src/engine.rc)

if (APPLE)
	list(APPEND ENGINE_GRAPHICS_HEADERS ${ENGINE_METAL_HEADERS})
	list(APPEND ENGINE_GRAPHICS_SOURCES ${ENGINE_METAL_SOURCES})
else()
	list(APPEND ENGINE_GRAPHICS_HEADERS ${ENGINE_VULKAN_HEADERS})
	list(APPEND ENGINE_GRAPHICS_SOURCES ${ENGINE_VULKAN_SOURCES})
endif()


if (WIN32)
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_WINDOWS_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_WINDOWS_SOURCES} ${ENGINE_WINDOWS_${CMAKE_HOST_SYSTEM_PROCESSOR}_SOURCES})
elseif (APPLE)
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_MACOS_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_MACOS_SOURCES} ${ENGINE_MACOS_${CMAKE_HOST_SYSTEM_PROCESSOR}_SOURCES})
else()
	set(ENGINE_PLATFORM_HEADERS ${ENGINE_LINUX_HEADERS})
	set(ENGINE_PLATFORM_SOURCES ${ENGINE_LINUX_SOURCES} ${ENGINE_LINUX_${CMAKE_HOST_SYSTEM_PROCESSOR}_SOURCES})
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

configure_file(include/purpl/core/config.h.in config.h @ONLY)

set(EXPORTS_HEADER ${CMAKE_CURRENT_BINARY_DIR}/exports.h)

add_library(engine_preinit STATIC src/core/main.c src/core/preinit.cc ${CMAKE_SOURCE_DIR}/tools/dumppe.py ${CMAKE_SOURCE_DIR}/tools/dumpelf.py ${CMAKE_SOURCE_DIR}/tools/dumpmacho.py ${EXPORTS_HEADER})
add_library(engine SHARED ${ENGINE_ALL_HEADERS} ${ENGINE_ALL_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/config.h)
foreach (warning ${_WARNINGS})
	target_compile_options(engine_preinit PRIVATE ${warning})
	target_compile_options(engine PRIVATE ${warning})
endforeach()
target_compile_definitions(engine_preinit PRIVATE PURPL_BUILD=1)
target_compile_definitions(engine PRIVATE PURPL_BUILD=1)
if (${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
	target_compile_definitions(engine_preinit PRIVATE PURPL_WINRT=1)
	target_compile_definitions(engine PRIVATE PURPL_WINRT=1)
endif()
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	target_compile_definitions(engine_preinit PRIVATE PURPL_DEBUG=1)
	target_compile_definitions(engine PRIVATE PURPL_DEBUG=1)
endif()

target_link_libraries(engine PRIVATE ${SDL2_LIBRARIES})

if (MSVC)
	target_compile_options(engine_preinit PRIVATE /wd4152)
elseif(NOT WIN32)
	set_target_properties(engine PROPERTIES C_VISIBILITY_PRESET hidden)
endif()

if (WIN32)
	add_custom_command(OUTPUT ${EXPORTS_HEADER}
			   COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/dumppe.py $<TARGET_FILE:engine> ${EXPORTS_HEADER}
			   DEPENDS engine ${CMAKE_SOURCE_DIR}/tools/dumppe.py)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR APPLE)
	if (APPLE)
		set(DUMPEXPORTS_NAME dumpmacho.py)
	else()
		set(DUMPEXPORTS_NAME dumpelf.py)
	endif()
	add_custom_command(OUTPUT ${EXPORTS_HEADER}
			   COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/${DUMPEXPORTS_NAME} $<TARGET_FILE:engine> ${EXPORTS_HEADER}
			   DEPENDS engine ${CMAKE_SOURCE_DIR}/tools/${DUMPEXPORTS_NAME})
	target_link_libraries(engine_preinit PUBLIC dl)
endif()

if (${PURPL_ENABLE_MEMING})
	target_compile_definitions(engine PRIVATE PURPL_ENABLE_MEMING=1)
endif()
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	target_compile_definitions(engine PRIVATE PURPL_X86=1 PURPL_X86_64=1)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "86")
	target_compile_definitions(engine PRIVATE PURPL_X86=1)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "a.*64")
	target_compile_definitions(engine PRIVATE PURPL_ARM=1)
endif()

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
target_link_libraries(engine PRIVATE SDL2::SDL2)
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
set_target_properties(engine PROPERTIES PREFIX "")
set_target_properties(engine_preinit PROPERTIES PREFIX "")

foreach(shader ${ENGINE_SHADERS})
	string(REPLACE "." "_" shader_name ${shader})
	string(FIND "${shader}" "/" shader_basename_start REVERSE)
	math(EXPR shader_basename_start "${shader_basename_start} + 1" OUTPUT_FORMAT DECIMAL)
	string(SUBSTRING "${shader_name}" ${shader_basename_start} -1 shader_basename)
	add_shader(${shader_basename} ${shader} assets/shaders)
	add_dependencies(engine ${shader_basename})
endforeach()

if (WIN32)
	set(ENGINE_RUNTIME_FILES_ $<TARGET_FILE:engine> $<TARGET_FILE:SDL2::SDL2>)
	if (MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Deb") # Debug and RelWithDebInfo both give PDB files
		list(APPEND ENGINE_RUNTIME_FILES_ $<TARGET_PDB_FILE:engine> $<TARGET_PDB_FILE:SDL2::SDL2>)
	endif()
    set(ENGINE_RUNTIME_FILES ${ENGINE_RUNTIME_FILES_} PARENT_SCOPE)
endif()

install(TARGETS engine_preinit
	LIBRARY
	DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(TARGETS engine
	LIBRARY
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

if (MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Deb")
	install(PROGRAMS $<TARGET_PDB_FILE:engine>
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

