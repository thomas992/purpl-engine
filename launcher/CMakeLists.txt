cmake_minimum_required(VERSION 3.23)

set(LAUNCHER_SOURCES main.c)

add_executable(launcher ${LAUNCHER_SOURCES})
target_include_directories(launcher PRIVATE ${PURPL_INCLUDE_DIRS})
target_link_libraries(launcher PRIVATE common libzstd_static SDL2main SDL2)

