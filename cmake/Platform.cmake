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

# Magic string for no content files
set(NO_CONTENT_STR "NO_CONTENT")

function(purpl_setup target sources)
	# Extra sources
	if (WIN32)
		set(sources ${sources} ${CMAKE_CURRENT_BINARY_DIR}/purpl-demo.rc purpl-demo.rc.in)
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
		set(sources ${sources} ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest uwp/Package.appxmanifest.in)
	elseif (ANDROID)
		set(sources ${sources} ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml ${CMAKE_CURRENT_BINARY_DIR}/strings.xml ${CMAKE_CURRENT_BINARY_DIR}/Entry.java android/AndroidManifest.xml.in android/strings.xml.in android/src/Entry.java.in)
	endif()

	# Create the target
	if (ANDROID)
		add_library(${target} SHARED ${sources})
	else()
		add_executable(${target} ${sources})
	endif()

	# Link to the engine
	target_link_libraries(${target} PRIVATE engine_preinit)
	if (${PURPL_STATIC_BUILD})
		target_link_libraries(${target} PRIVATE engine)
	endif()

	if (NOT ${PURPL_STATIC_BUILD})
		# Copy the list of engine libraries
		foreach (dest ${CMAKE_CURRENT_BINARY_DIR} $<TARGET_FILE_DIR:${target}>)
			add_custom_command(TARGET purpl-demo PRE_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/engine/engine_libs.txt ${dest})
		endforeach()
	endif()
	
	# Content files are optional
	if (${ARGC} GREATER 4)
		set(content_files ${ARG5})
	else()
		set(content_files ${NO_CONTENT_STR})
	endif()

	# Platform-specific setup
	if (WIN32)
		purpl_win32_setup(${target})
	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
		purpl_winrt_setup(${content_files})
	elseif (ANDROID)
		purpl_android_setup(${target} ${content_files})
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
	if (NOT ${content_files} STREQUAL ${NO_CONTENT_STR})
		foreach (file ${content_files})
			get_filename_component(dir ${file} DIRECTORY)
			install(FILES ${file}
				DESTINATION ${CMAKE_INSTALL_PREFIX}/${dir})
		endforeach()
	endif()
endfunction()

function(purpl_android_setup target content_files)
	set(apk_dir ${CMAKE_CURRENT_BINARY_DIR}/android-project/app/build/outputs/apk)
	set(jni_dir ${CMAKE_CURRENT_BINARY_DIR}/android-project/app/jni)
	set(src_dir ${CMAKE_CURRENT_BINARY_DIR}/android-project/app/src/main)
	set(content_dir ${src_dir}/res)

	configure_file(android/AndroidManifest.xml.in AndroidManifest.xml @ONLY)
	configure_file(android/Android.mk.in Android.mk @ONLY)
	configure_file(android/build.gradle.in build.gradle @ONLY)
	configure_file(android/strings.xml.in strings.xml @ONLY)
	configure_file(android/src/Entry.java.in Entry.java @ONLY)

	string(REPLACE "." "/" java_package "${PURPL_APP_JAVA_ID}")

	get_filename_component(java_dir ${Java_JAVA_EXECUTABLE} DIRECTORY)
	file(REAL_PATH .. java_home BASE_DIRECTORY ${java_dir})

	add_custom_target(${target}-apk ALL
			  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/deps/sdl2/android-project ${CMAKE_CURRENT_BINARY_DIR}/android-project
			  COMMAND ${CMAKE_COMMAND} -E rm -rf ${jni_dir}
			  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/build.gradle ${CMAKE_CURRENT_BINARY_DIR}/android-project/app/build.gradle
			  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/Android.mk ${jni_dir}/Android.mk
			  COMMAND ${CMAKE_COMMAND} -E make_directory ${jni_dir}/${ANDROID_ABI}
			  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${target}> ${jni_dir}/${ANDROID_ABI}/$<TARGET_FILE_NAME:${target}>
			  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml ${src_dir}/AndroidManifest.xml
			  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/strings.xml ${content_dir}/values/strings.xml
			  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/Entry.java ${src_dir}/java/${java_package}/${PURPL_APP_UPPER_ID}.java
			  COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR}/android-project ${CMAKE_COMMAND} -E env ANDROID_SDK_ROOT=${PURPL_ANDROID_SDK} JAVA_HOME=${java_home} ./gradlew build
			  COMMAND ${CMAKE_COMMAND} -E copy ${apk_dir}/debug/app-debug.apk ${CMAKE_CURRENT_BINARY_DIR}/${target}-debug.apk
			  COMMAND ${CMAKE_COMMAND} -E copy ${apk_dir}/release/app-release-unsigned.apk ${CMAKE_CURRENT_BINARY_DIR}/${target}-release.apk
			  DEPENDS ${target}
			  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/android-project/app/build)


	if (NOT ${content_files} STREQUAL ${NO_CONTENT_STR})
		foreach(file ${content_files})
			get_filename_component(dir ${file} DIRECTORY)
			add_custom_command(TARGET ${target} POST_BUILD
					   COMMAND ${CMAKE_COMMAND} -E make_directory ${src_dir}/${dir}
					   COMMAND ${CMAKE_COMMAND} -E copy ${file} ${src_dir}/${dir})
		endforeach()
	endif()

	add_custom_command(TARGET ${target} POST_BUILD
			   COMMAND ${CMAKE_COMMAND} -E make_directory ${src_dir}/assets
			   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/engine/assets ${src_dir}/assets)
endfunction()

function(purpl_posix_setup target)
endfunction()

function(purpl_win32_setup target)
	string(REPLACE "." "," version_string "${PURPL_APP_VERSION}")
	configure_file(${PURPL_APP_ID}.rc.in ${PURPL_APP_ID}.rc @ONLY)
	set_property(SOURCE ${PURPL_APP_ID}.rc PROPERTY GENERATED TRUE)

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
	configure_file(uwp/Package.appxmanifest.in Package.appxmanifest @ONLY)
	set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest PROPERTY VS_DEPLOYMENT_CONTENT TRUE)
	set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest PROPERTY GENERATED TRUE)

	foreach(file icon_44.png icon_50.png icon_150.png splash.png)
		set_property(SOURCE uwp/${file} PROPERTY VS_DEPLOYMENT_CONTENT TRUE)
		set_property(SOURCE uwp/${file} PROPERTY VS_DEPLOYMENT_LOCATION "Assets")
	endforeach()

	configure_file(uwp/create_test_cert.ps1.in create_test_cert.ps1 @ONLY)
	message(STATUS "To create a certificate, run the following command: ${CMAKE_CURRENT_BINARY_DIR}/create_test_cert.ps1")
endfunction()
