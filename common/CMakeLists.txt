cmake_minimum_required(VERSION 3.23)

set(COMMON_HEADERS dll.h
		   gameinfo.h
		   ini.h
		   pack.h
		   util.h
		   xxhash.h)
set(COMMON_SOURCES dll.c
		   gameinfo.c
		   ini.c
		   pack.c
		   util.c)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(COMMON_SOURCES ${COMMON_SOURCES}
			   win32_dll.c)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(COMMON_SOURCES ${COMMON_SOURCES}
			   linux_dll.c)
endif()

add_library(common STATIC ${COMMON_HEADERS} ${COMMON_SOURCES})
target_include_directories(common PRIVATE ${PURPL_INCLUDE_DIRS})

