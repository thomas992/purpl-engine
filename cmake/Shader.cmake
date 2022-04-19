cmake_minimum_required(VERSION 3.15)

function(add_shader target shader shaders_dir)
	if (NOT DEFINED Vulkan_GLSLC_EXECUTABLE)
		message(FATAL_ERROR "glslc executable not found")
	endif()
	if (NOT DEFINED SPIRV_CROSS_EXECUTABLE)
		message(FATAL_ERROR "spirv-cross executable not found")
	endif()

	set(current_shader_path ${CMAKE_CURRENT_SOURCE_DIR}/${shader})
	set(current_output_path ${CMAKE_CURRENT_BINARY_DIR}/${target}.spv)

	# Add a custom command to compile GLSL to SPIR_V.
	get_filename_component(current_output_dir ${current_output_path} DIRECTORY)
	file(MAKE_DIRECTORY ${current_output_dir})

	foreach(shader_type comp;frag;geom;vert)
		string(FIND ${current_output_path} "${shader_type}" index)
		if (NOT ${index} EQUAL -1)
			string(SUBSTRING "${shader_type}" 0 1 shader_type_0)
			string(REPLACE "_${shader_type}.spv" ".${shader_type_0}sh" hlsl_output ${current_output_path})
		endif()
	endforeach()
	string(REPLACE ".spv" ".metal" msl_output ${current_output_path})

	add_custom_target(${target}
			  COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${current_output_path} ${current_shader_path}
			  COMMAND ${SPIRV_CROSS_EXECUTABLE} --hlsl --shader-model 50 ${current_output_path} --output ${hlsl_output}
			  COMMAND ${SPIRV_CROSS_EXECUTABLE} --msl ${current_output_path} --output ${msl_output}
			  DEPENDS ${SPIRV_CROSS_EXECUTABLE}
			  BYPRODUCTS ${current_output_path}
			  VERBATIM
			  SOURCES ${current_shader_path})

	install(FILES ${current_output_path}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/spirv)
	install(FILES ${hlsl_output}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/directx)
	install(FILES ${msl_output}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/metal)
endfunction(add_shader)

