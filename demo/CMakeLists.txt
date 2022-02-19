cmake_minimum_required(VERSION 3.10)

set(DEMO_SOURCES src/main.c)

add_executable(demo ${DEMO_SOURCES})
target_link_libraries(demo PRIVATE engine)

if (WIN32)
	add_custom_command(TARGET demo
					   POST_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:demo> $<TARGET_FILE_DIR:demo>
					   COMMAND_EXPAND_LISTS)
endif()

install(TARGETS demo
	RUNTIME
	DESTINATION ${CMAKE_INSTALL_PREFIX})
