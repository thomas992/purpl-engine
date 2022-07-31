cmake_minimum_required(VERSION 3.22)

set(LAUNCHER_SOURCES main.c)

add_executable(launcher ${LAUNCHER_SOURCES})
target_include_directories(launcher PRIVATE ${PURPL_INCLUDE_DIRS})
target_link_libraries(launcher PRIVATE common libzstd_static SDL2main SDL2)
copy_libs(launcher ${ENGINE_LIBS})

add_custom_command(TARGET launcher
		   POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:launcher>/core
		   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/game/data/core $<TARGET_FILE_DIR:launcher>/core
		   COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:launcher>/purpl
		   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/game/data/purpl $<TARGET_FILE_DIR:launcher>/purpl)
