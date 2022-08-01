cmake_minimum_required(VERSION 3.22)

function(copy_libs target)
	if ("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
		foreach (lib ${ARGN})
			set(extra_libs ${extra_libs} $<TARGET_FILE:${lib}>)
		endforeach()
		set(script ${CMAKE_CURRENT_BINARY_DIR}/${target}_copy_libs_$<CONFIG>.cmake)
		string(CONCAT script_content "set(libs $<TARGET_RUNTIME_DLLS:${target}>)\n"
					     "foreach(lib ${libs})\n"
					     "\tfile(COPY_FILE ${lib} $<TARGET_FILE_DIR:${target}>/$<PATH:GET_FILENAME,${lib}$<ANGLE-R>)\n"
					     "endforeach()\n")
		file(GENERATE OUTPUT ${script}
			      CONTENT ${script_content})
		add_custom_target(${target}_copy_libs
				  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>"
				  COMMAND ${CMAKE_COMMAND} -P ${script}
				  COMMAND_EXPAND_LISTS)
		foreach (lib ${ARGN})
			add_dependencies(${target}_copy_libs ${lib})
		endforeach()
		add_dependencies(${target} ${target}_copy_libs)
	endif()
endfunction()
