cmake_minimum_required(VERSION 3.22)

function(copy_libs target)
	if ("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
		foreach (lib ${ARGN})
			set(extra_libs ${extra_libs} $<TARGET_FILE:${lib}>)
		endforeach()
		add_custom_target(${target}_copy_libs
				  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>"
				  COMMAND_EXPAND_LISTS)
		foreach (lib ${ARGN})
			add_dependencies(${target}_copy_libs ${lib})
			add_custom_command(TARGET ${target}_copy_libs POST_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${lib}>" "$<TARGET_FILE_DIR:${target}>")
		endforeach()
		add_dependencies(${target} ${target}_copy_libs)
	endif()
endfunction()
