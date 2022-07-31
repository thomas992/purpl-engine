cmake_minimum_required(VERSION 3.22)

function(copy_libs target)
	if ("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
		foreach (lib ${ARGN})
			set(extra_libs ${extra_libs} $<TARGET_FILE:${lib}>)
		endforeach()
		add_custom_target(${target}_copy_libs
				  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>"
				  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${target}> ${extra_libs} "$<TARGET_FILE_DIR:${target}>"
				  COMMAND_EXPAND_LISTS)
		foreach (lib ${ARGN})
			add_dependencies(${target}_copy_libs ${lib})
		endforeach()
		add_dependencies(${target} ${target}_copy_libs)
	endif()
endfunction()
