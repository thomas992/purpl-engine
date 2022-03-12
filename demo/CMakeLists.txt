cmake_minimum_required(VERSION 3.10)

set(DEMO_SOURCES src/main.c)

add_executable(demo ${DEMO_SOURCES})
target_link_libraries(demo PRIVATE engine)

if (WIN32)
	add_custom_command(TARGET demo
			   POST_BUILD
			   COMMAND ${CMAKE_COMMAND} -E copy ${ENGINE_RUNTIME_FILES} $<TARGET_FILE_DIR:demo>
			   DEPENDS ${ENGINE_RUNTIME_FILES})
endif()

install(TARGETS demo
	RUNTIME
	DESTINATION ${CMAKE_INSTALL_PREFIX})
