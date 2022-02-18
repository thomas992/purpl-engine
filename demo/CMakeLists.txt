cmake_minimum_required(VERSION 3.10)

add_executable(demo src/main.c)
target_link_libraries(demo PRIVATE engine)

install(TARGETS demo
	RUNTIME
	DESTINATION ${CMAKE_INSTALL_PREFIX})
