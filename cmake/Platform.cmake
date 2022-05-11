#  Platform setup functions for applications
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

function(purpl_setup target app_id version content_files)
	# Copy the list of engine libraries
	foreach (dest ${CMAKE_CURRENT_BINARY_DIR} $<TARGET_FILE_DIR:${target}>)
		add_custom_command(TARGET purpl-demo PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/engine/engine_libs.txt ${dest})
	endforeach()
	
	# Platform-specific setup
	if (WIN32)
		purpl_win32_setup(${target} ${app_id} ${version})
	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
		purpl_winrt_setup(${content_files})
	else()
		purpl_posix_setup(${target})
	endif()

	# Install the application
	install(TARGETS ${target}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
	
	# Install debug symbols
	if (MSVC AND ${PURPL_IS_DEBUG})
		install(PROGRAMS $<TARGET_PDB_FILE:${target}>
			DESTINATION ${CMAKE_INSTALL_PREFIX})
	endif()

	# Install assets
	foreach (file ${content_files})
		get_filename_component(dir ${file} DIRECTORY)
		install(FILES ${file}
			DESTINATION ${CMAKE_INSTALL_PREFIX}/${dir})
	endforeach()
endfunction()

function(purpl_posix_setup target)
endfunction()

function(purpl_win32_setup target app_id version)
	string(REPLACE "." "," version_string "${version}")
	configure_file(${app_id}.rc.in ${app_id}.rc @ONLY)
	set_property(SOURCE ${app_id}.rc PROPERTY GENERATED TRUE)

	# Copy DLLs
	add_custom_command(TARGET purpl-demo PRE_BUILD
			   COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target}>)
	foreach (runtime_target ${ENGINE_RUNTIME_TARGETS} engine)
		add_custom_command(TARGET purpl-demo PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${runtime_target}> $<TARGET_FILE_DIR:${target}>
				   DEPENDS ${runtime_target})
		if (MSVC AND ${PURPL_IS_DEBUG})
			get_target_property(target_imported ${runtime_target} IMPORTED)
			if (NOT ${target_imported})
				add_custom_command(TARGET purpl-demo PRE_BUILD
						   COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${runtime_target}> $<TARGET_FILE_DIR:${target}>
						   DEPENDS ${target})
			endif()
		endif()
	endforeach()
endfunction()

function(purpl_winrt_setup content_files)
	configure_file(Package.appxmanifest.in Package.appxmanifest @ONLY)
	set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest PROPERTY VS_DEPLOYMENT_CONTENT TRUE)
	set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest PROPERTY GENERATED TRUE)

	foreach(file ${content_files})
		set_property(SOURCE ${file} PROPERTY VS_DEPLOYMENT_CONTENT TRUE)
		set_property(SOURCE ${file} PROPERTY VS_DEPLOYMENT_LOCATION "Assets")
	endforeach()

	configure_file(create_test_cert.ps1.in create_test_cert.ps1 @ONLY)
	message(STATUS "To create a certificate, run the following command: ${CMAKE_CURRENT_BINARY_DIR}/create_test_cert.ps1")
endfunction()
