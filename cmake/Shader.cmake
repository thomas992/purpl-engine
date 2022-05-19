#  Shader compilation. This is modified from a Reddit post
#
#  Copyright 2022 MobSlicer152
#  This file is part of Purpl Engine
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

cmake_minimum_required(VERSION 3.15)

function(add_shader target shader shaders_dir)
	if (NOT DEFINED Vulkan_GLSLC_EXECUTABLE)
		message(FATAL_ERROR "glslc executable not found")
	endif()
	if (NOT DEFINED SPIRV_CROSS_EXECUTABLE)
		message(FATAL_ERROR "spirv-cross executable not found")
	endif()

	set(current_shader_path ${CMAKE_CURRENT_SOURCE_DIR}/${shader})
	set(current_output_path ${target}.spv)

	# Add a custom command to compile GLSL to SPIR_V.
	foreach(shader_type comp;frag;geom;vert)
		string(FIND ${current_output_path} "${shader_type}" index)
		if (NOT ${index} EQUAL -1)
			string(SUBSTRING "${shader_type}" 0 1 shader_type_0)
			string(REPLACE "_${shader_type}.spv" ".${shader_type_0}sh" hlsl_output ${current_output_path})
		endif()
	endforeach()
	string(REPLACE ".spv" ".metal" msl_output ${current_output_path})

	add_custom_target(${target}
			  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/spirv
			  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/directx
			  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/metal
			  COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/spirv/${current_output_path} ${current_shader_path}
			  COMMAND ${SPIRV_CROSS_EXECUTABLE} --hlsl --shader-model 50 ${current_output_path} --output ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/directx/${hlsl_output}
			  COMMAND ${SPIRV_CROSS_EXECUTABLE} --msl ${current_output_path} --output ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/metal/${msl_output}
			  DEPENDS ${SPIRV_CROSS_EXECUTABLE}
			  BYPRODUCTS ${current_output_path}
			  VERBATIM
			  SOURCES ${current_shader_path})

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/spirv/${current_output_path}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/spirv)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/directx/${hlsl_output}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/directx)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${shaders_dir}/metal/${msl_output}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${shaders_dir}/metal)
endfunction(add_shader)


