cmake_minimum_required(VERSION 3.22)

set(PAKTOOL_SOURCES main.c)
add_executable(paktool ${PAKTOOL_SOURCES})
target_compile_definitions(paktool PRIVATE SDL_MAIN_HANDLED=1)
target_include_directories(paktool PRIVATE ${PURPL_INCLUDE_DIRS})
target_link_libraries(paktool PRIVATE common libzstd_shared)
copy_libs(paktool)

