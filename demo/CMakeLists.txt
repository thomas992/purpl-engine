cmake_minimum_required(VERSION 3.10)

set(DEMO_SOURCES src/main.c)

add_executable(demo ${DEMO_SOURCES})

# This library handles loading the shared library
target_link_libraries(demo PRIVATE engine_preinit)

if (WIN32)
	add_custom_command(TARGET demo PRE_BUILD
			   COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:demo>
			   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ENGINE_RUNTIME_FILES} $<TARGET_FILE_DIR:demo>
			   DEPENDS ${ENGINE_RUNTIME_FILES})
endif()

install(TARGETS demo
	RUNTIME
	DESTINATION ${CMAKE_INSTALL_PREFIX})

if (MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Deb")
	install(PROGRAMS $<TARGET_PDB_FILE:demo>
		DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
