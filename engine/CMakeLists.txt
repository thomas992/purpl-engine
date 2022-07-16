cmake_minimum_required(VERSION 3.23)

set(ENGINE_HEADERS engine.h
		   render.h)

set(ENGINE_SOURCES engine.c
		   render.c)

# DirectX
if ("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
	set(ENGINE_HEADERS ${ENGINE_HEADERS}
			   dx_stuff.h)
	set(ENGINE_SOURCES ${ENGINE_SOURCES}
			   dx_device.c
			   dx_main.c)
	message(STATUS "Enabling DirectX support")
	set(ENGINE_DIRECTX TRUE)
endif()

# Vulkan
if (NOT APPLE)
	set(ENGINE_HEADERS ${ENGINE_HEADERS}
			   vk_glad.h
			   vk_glad_platform.h
			   vk_stuff.h)
	set(ENGINE_SOURCES ${ENGINE_SOURCES}
			   vk_device.c
			   vk_glad.c
			   vk_main.c)
	message(STATUS "Enabling Vulkan support")
	set(ENGINE_VULKAN TRUE)
endif()

if (NOT DEFINED ENGINE_DIRECTX)
	set(ENGINE_DIRECTX FALSE)
endif()
if (NOT DEFINED ENGINE_VULKAN)
	set(ENGINE_VULKAN FALSE)
endif()

add_library(engine SHARED ${ENGINE_HEADERS} ${ENGINE_SOURCES})
target_include_directories(engine PRIVATE ${PURPL_INCLUDE_DIRS})
target_link_libraries(engine PRIVATE common flecs SDL2 libzstd_shared)
if (${ENGINE_DIRECTX})
	target_link_libraries(engine PRIVATE d3d12.lib dxgi.lib dxguid.lib)
endif()
if (${ENGINE_VULKAN})
	find_package(Vulkan REQUIRED)
	target_include_directories(engine PRIVATE ${Vulkan_INCLUDE_DIRS})
endif()

