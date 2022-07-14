cmake_minimum_required(VERSION 3.23)

function(copy_libs target)
	if ("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
		foreach (lib ${ARGN})
			add_custom_command(TARGET ${target}
					   POST_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${lib}>" "$<TARGET_FILE_DIR:${target}>")
		endforeach()
	endif()
endfunction()

