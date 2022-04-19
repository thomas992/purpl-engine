cmake_minimum_required(VERSION 3.15)

function(add_shader target shader shaders_dir)
	if (NOT DEFINED Vulkan_GLSLC_EXECUTABLE)
		message(FATAL_ERROR "glslc executable not found")
	endif()
	if (NOT DEFINED SPIRV_CROSS_EXECUTABLE)
		message(FATAL_ERROR "spirv-cross executable not found")
	endif()

	set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/${shader})
	set(current-output-path ${CMAKE_CURRENT_BINARY_DIR}/${target}.spv)

	# Add a custom command to compile GLSL to SPIR-V.
	get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
	file(MAKE_DIRECTORY ${current-output-dir})

	string(FIND ${current-output-path} "vert" vert_index)
	string(FIND ${current-output-path} "frag" frag_index)
	if (NOT ${vert_index} EQUAL -1)
		string(REPLACE "_vert.spv" ".vsh" hlsl_output ${current-output-path})
	elseif(NOT ${frag_index} EQUAL -1)
		string(REPLACE "_frag.spv" ".fsh" hlsl_output ${current-output-path})
	endif()
	string(REPLACE ".spv" ".metal" msl_output ${current-output-path})

	add_custom_target(${target}
			  COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${current-output-path} ${current-shader-path}
			  COMMAND ${SPIRV_CROSS_EXECUTABLE} --hlsl --shader-model 50 ${current-output-path} --output ${hlsl_output}
			  COMMAND ${SPIRV_CROSS_EXECUTABLE} --msl ${current-output-path} --output ${msl_output}
			  COMMENT "Building shader object ${current-shader-path}"
			  DEPENDS ${SPIRV_CROSS_EXECUTABLE}
			  BYPRODUCTS ${current-output-path}
			  VERBATIM
			  SOURCES ${current-shader-path})

	install(FILES ${current-output-path}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/spirv)
	install(FILES ${hlsl_output}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/directx)
	install(FILES ${msl_output}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/metal)
endfunction(add_shader)

